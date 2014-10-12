 from jobsimulator import *
import simplejson as json
import operator
import random



    

class TaskSim:
    def __init__(self):
        self.tasks = dict() #{agent:[{id:taskid,last:time_last,demands:{resource type: demand}}]}
        self.time = 0
        self.timer = dict() #{finish time: [agent,task id]}

    #task: {agent,task:{id,last,demands}}
    def addTask(self, task):
        try:
            self.tasks[task['agent']].append(task['task'])
        except:
            self.tasks[task['agent']] = [task['task']]

        try:
            self.timer[task['task']['last'] + self.time].append([task['agent'],task['task']['taskid']])
        except:
            self.timer[task['task']['last'] + self.time] = [[task['agent'],task['task']['taskid']]]

    #tasks: [{agent, task id, last, demands:{resource type:amount}}]
    def addTasks(self,tasks):
        for task in tasks:
            self.addTask({'agent':task['agent'], 'task':{'taskid':task['taskid'],'last':task['last'],'demands':task['demands']}})

    def step(self):
        if self.time in self.timer:
            for tsk in self.timer[self.time]:
                cnt = 0
                for tsk2 in self.tasks[tsk[0]]:
                    if tsk[1] == tsk2['taskid']:
                        yield {'agent': tsk[0], 'task': tsk2}
                        del self.tasks[tsk[0]][cnt]
                        break
                    cnt = cnt + 1
            del self.timer[self.time]
        self.time = self.time + 1
        


class Worker(Node):

    def __init__(self,name,env,sched_name):
        super(Worker,self).__init__(name,env)
        self.resources = dict()  #{resource type: amount}
        self.tasks = TaskSim()     
        self.consumed = dict()    #{resource type: amount}
        self.sched_name = sched_name

    def setResource(self,res):
        for r in res:
            self.resources[r] = res[r]
            self.consumed[r] = 0

    def execNodeEvent(self,event,msg):
        if 'start' == event:
            self.setRun()
            self.env.send(self.sched_name, {'from':self.name, 'type':'resouce_add','msg':self.resources})
            return

        if 'stop' == event:
            self.setStop()
            self.env.send(self.sched_name,{'from':self.name,'type':'resource_remove','msg':self.resources})
            return


    def step(self):

        self.processMsg()
        self.innerStateUpdate()
        

    def checkConstraints(self, tasks):
        try:
            tasks_consume = dict()

            for task in tasks:
                for res in task['demands']:
                    if res in tasks_consume:
                        tasks_consume[res] = tasks_consume[res] + task['demands'][res]
                    else:
                        tasks_consume[res] = task['demands'][res]
            for res in tasks_consume:                      
                if tasks_consume[res] + self.consumed[res] > self.resources[res]: 
                    return False
        except:
            print 'error in checkConstraints'
            return False
        return True

    def calPrice(self,tasks):
        return int(self.name[-1])

    def available(self):
        available = dict()
        for res in self.resources:
            available[res] = self.resources[res] - self.consumed[res]
        return available
        
        

    def processMsg(self):
        msg = self.msgs.getMsg()
        while msg != None:
            if msg['from'] == self.sched_name:
                #task message format {agent, taskid, last, demands:{resource type:amount}}
                if msg['type'] == 'task_offer':
                    if self.checkConstraints([msg['msg']]):
                        self.env.send(self.sched_name, {'from':self.name, 'type':'task_price','taskid':msg['msg']['taskid'],'msg':self.calPrice(msg['msg'])})
                    else:
                        self.env.send(self.sched_name, {'from':self.name, 'type':'task_reject','taskid':msg['msg']['taskid']})

                if msg['type'] == 'task_assign':
                    
                    if self.checkConstraints([msg['msg']]):

                        self.env.send(self.sched_name, {'from':self.name, 'type':'task_accept','taskid':msg['msg']['taskid']})
                        self.tasks.addTasks([msg['msg']])
                        
                        for res in msg['msg']['demands']:
                            self.consumed[res] = self.consumed[res] + msg['msg']['demands'][res]
                    
                    else:
                        self.env.send(self.sched_name, {'from':self.name, 'type':'task_reject','taskid':msg['msg']['taskid']})
            msg = self.msgs.getMsg()
        
    
    def innerStateUpdate(self):
        for cmpltask in self.tasks.step():
            for res in cmpltask['task']['demands']:
                self.consumed[res] = self.consumed[res] - cmpltask['task']['demands'][res]
            self.env.send(self.sched_name, {'from':self.name, 'type':'task_finish','msg':{'agent':cmpltask['agent'],'taskid':cmpltask['task']['taskid']}})
        
 ##       self.env.send(sched_name, {'from':self.name, 'type':'resource_update','msg':self.available()}                                 


class ResourceTracker:
    def __init__(self):
        self.wait_agents = dict() #{agent:{task:{last,demands}}}
        self.alloc_tasks = dict()    #{agent:{task:{start,last,node,demands}}}
        self.alloc_resources = dict()   #{agent:{resource type: amount}}
        self.resources = dict()  #total resources: {resouce type: amount}
        self.node_resources = dict() #available resource of each node: {node:{resource type: amount}}
 #       self.hold_tasks = dict()    #
        self.expagents = 1
        self.allgents = 1
        self.count_fintasks = 0

    def addAgent(self,newagent, t):
        self.wait_agents[newagent['agent']]=newagent['tasks']
        self.alloc_resources[newagent['agent']] = dict()
        for taskid in newagent['tasks']:
            for res in newagent['tasks'][taskid]['demands']:
                self.alloc_resources[newagent['agent']][res] = 0
        self.onAgentAdd(newagent,t)

    def allocateTask(self, agentid, taskid, stime,nodeid):
        if agentid not in self.alloc_tasks:
            self.alloc_tasks[agentid] = dict()
        self.alloc_tasks[agentid][taskid]= self.wait_agents[agentid][taskid]
        self.alloc_tasks[agentid][taskid]['start'] = stime
        self.alloc_tasks[agentid][taskid]['node'] = nodeid
        if agentid not in self.alloc_resources:
            self.alloc_resources[agentid] = dict()
            for res in self.reources:
                self.alloc_resources[agentid][res] = 0

        logstr = 'task add: jid ' + str(agentid) + ' taskid ' + str(taskid) + ' ('
        for res in self.wait_agents[agentid][taskid]['demands']:
            self.alloc_resources[agentid][res] = self.alloc_resources[agentid][res] + self.wait_agents[agentid][taskid]['demands'][res]
            self.node_resources[nodeid][res] = self.node_resources[nodeid][res] - self.wait_agents[agentid][taskid]['demands'][res]
            logstr = logstr + ' ' + res + ':' + str(self.wait_agents[agentid][taskid]['demands'][res])

        del self.wait_agents[agentid][taskid]
        self.log.log(logstr + ')')

    def endTask(self, agentid, taskid):
        logstr = '('
        nodeid = self.alloc_tasks[agentid][taskid]['node']
        for res in self.alloc_tasks[agentid][taskid]['demands']:
            self.alloc_resources[agentid][res] = self.alloc_resources[agentid][res] - self.alloc_tasks[agentid][taskid]['demands'][res]
            self.node_resources[nodeid][res] = self.node_resources[nodeid][res] + self.alloc_tasks[agentid][taskid]['demands'][res]
            logstr = logstr + ' ' + res + ':' + str(self.alloc_tasks[agentid][taskid]['demands'][res])
        del self.alloc_tasks[agentid][taskid]
        if self.wait_agents[agentid] == {} and self.alloc_tasks[agentid] == {}:
            self.removeAgent(agentid)
        self.log.log('task finish: jid ' + str(agentid) + ' taskid ' + str(taskid) + ' ' + logstr + ')')
        self.count_fintasks = self.count_fintasks + 1

    def removeAgent(self,agentid):
        try:
            self.onAgentRemove(agentid)
            del self.wait_agents[agentid]
            del self.alloc_resources[agentid]
            del self.alloc_tasks[agentid]
        except:
            pass

    def addResource(self,nodeid,resources):
        self.node_resources[nodeid] = dict(resources)
        for res in resources:
            if res in self.resources:
                self.resources[res] = self.resources[res] + resources[res]
            else:
                self.resources[res] = resources[res]

    def removeResource(self,nodeid,resources):
        for res in resources:
            self.resources[res] = self.resources[res] - resources[res]
        for agentid in list(self.alloc_tasks.keys()):
            for taskid in list(self.alloc_tasks[agentid].keys()):
                if self.alloc_tasks[agentid][taskid]['node'] == nodeid:
                    self.ceaseTask(agentid,taskid)
        del self.node_resources[nodeid]

    def ceaseTask(self, agentid, taskid):
        nodeid = self.alloc_tasks[agentid][taskid]['node']
        self.wait_agents[agentid][taskid] = {'last':self.alloc_tasks[agentid][taskid]['last'],'demands':self.alloc_tasks[agentid][taskid]['demands']}
        for res in self.alloc_tasks[agentid][taskid]['demands']:
            self.alloc_resources[agentid][res] = self.alloc_resources[agentid][res] - self.alloc_tasks[agentid][taskid]['demands'][res]
            self.node_resources[nodeid][res] = self.node_resources[nodeid][res] + self.alloc_tasks[agentid][taskid]['demands'][res]
        del self.alloc_tasks[agentid][taskid]

    def dominateResource(self,agentid):
        if agentid in self.alloc_resources:
            res_ratio = {res : float(self.alloc_resources[agentid][res])/self.resources[res] for res in self.alloc_resources[agentid]}
            return max(res_ratio.iteritems(), key=operator.itemgetter(1))
        return None
        
    def sortedDR(self):
        return sorted([(agentid,self.dominateResource(agentid)[1]) for agentid in self.wait_agents], key=operator.itemgetter(1))

    def getTask(self,agentid):
        if agentid in self.wait_agents and self.wait_agents[agentid] != {}:
            taskid = self.wait_agents[agentid].keys()[0]
            return (taskid, self.wait_agents[agentid][taskid])
        return None

    def consumedResources(self):
        com = dict()
        for agentid in self.alloc_resources:
            for res in self.alloc_resources[agentid]:
                if res in com:
                    com[res] = com[res] + self.alloc_resources[agentid][res]
                else:
                    com[res] = self.alloc_resources[agentid][res]
        return com

    def dynamicConstraints(self, nodeid, agentid, taskid):
        consumed = self.consumedResources()
        num_agents = len(self.wait_agents) #+ len(self.alloc_tasks)
        if self.expagents < num_agents:
            self.expagents = num_agents
        ratio = float(num_agents)/self.expagents
        for res in self.wait_agents[agentid][taskid]['demands']:
            if ratio * self.resources[res] < self.wait_agents[agentid][taskid]['demands'][res] + consumed[res]:
                return False
        return True
            

    
    def tryAllocate(self, nodeid, agentid, taskid):
        for res in self.node_resources[nodeid]:
            if self.wait_agents[agentid][taskid]['demands'][res] > self.node_resources[nodeid][res]:
                return False
        if not self.dynamicConstraints(nodeid,agentid,taskid):
            return False
        return True


    def envyPairs(self):
        aglist = self.alloc_resources.keys()
        for i1 in range(0,len(aglist)):
            a1 = aglist[i1]
            for i2 in range(i1 + 1,len(aglist)):
                a2 = aglist[i2]
                s1 = True
                for res in self.alloc_resources[a1]:
                    if self.alloc_resources[a1] < self.alloc_resources[a2]:
                        s1 = False
                        break
                if s1:
                    continue
                    
                for res in self.alloc_resources[a1]:
                    if self.alloc_resources[a1] > self.alloc_resources[a2]:
                        yield [a1,a2]
                        break
            
    def onAgentAdd(self,newagent, t):
        pass

    def onAgentRemove(self, agentid):
        pass
    

    def status(self):
        com = self.consumedResources()

        usage = []
        for res in com:
            self.log.log(res + " usage: " + str(float(com[res])/self.resources[res]) + ' ' + str(com[res]) + '/' + str(self.resources[res]))
            usage.append(float(com[res])/self.resources[res])
        self.log.logdata(usage)

        epairs = 0
        for p in self.envyPairs():
            self.log.log('envy pair: (' + p[0] + ',' + p[1] + ')')
            epairs = epairs + 1
        self.log.logdata(epairs)
        domstatus = []
        for i in range(self.allagents):
            aid = 'j' + str(i)
            agt = self.dominateResource(aid)
            if agt != None:
                domstatus.append(agt[1])
            else:
                domstatus.append(-1)

        self.log.logdata(domstatus)

        

        
        
        

    def setLog(self, log):
        self.log = log
    

    
        
        

class SchedMaster(Node):
    def __init__(self,name,env):
        super(SchedMaster,self).__init__(name,env)
        self.restrk = ResourceTracker()
        self.env = env


    def setLog(self,log):
        super(SchedMaster,self).setLog(log)
        self.restrk.setLog(log)
        

    def setExpectagentNum(self,ej):
        self.restrk.expagents = ej

    def fillNode(self, nodeid):
        while True:
            drs = self.restrk.sortedDR()
            if drs == []:
                return
            task = self.restrk.getTask(drs[0][0])
            agentid = ''
            
            for t in drs:
                task = self.restrk.getTask(t[0])
                if task != None:
                    agentid = t[0]
                    break
            
            if task == None or not self.restrk.tryAllocate(nodeid,agentid,task[0]):
                return
            self.env.send(nodeid,{'from':self.name,'type':'task_assign','msg':{'agent':agentid,'taskid':task[0],'last':task[1]['last'],'demands':task[1]['demands']}})
            self.restrk.allocateTask(agentid,task[0],self.env.time,nodeid)

    def processMsg(self):
        msg = self.msgs.getMsg()
        while msg != None:
            if msg['type'] == 'resouce_add':
                self.restrk.addResource(msg['from'],msg['msg'])
 #               self.fillNode(msg['from'])
            if msg['type'] == 'resouce_remove':
                self.restrk.removeResource(msg['from'],msg['msg'])
            if msg['type'] == 'task_finish':
                self.restrk.endTask(msg['msg']['agent'],msg['msg']['taskid'])

            
            msg = self.msgs.getMsg()


    def execNodeEvent(self,event,msg):
        if 'start' == event:
            self.setRun()
            return

        if 'stop' == event:
            self.setStop()
            return

        if 'addagent' == event:
            self.restrk.addAgent(msg,self.env.time)
            return

    def step(self):
        self.processMsg()
        for node in self.restrk.node_resources:
            self.fillNode(node)
        self.restrk.status()
    

        
def testR():
    network = SimEnvironment()
    worker1 = Worker('w1',network,'m')
    worker2 = Worker('w2',network,'m')
    master = SchedMaster('m',network)
    
    worker1.setResource({'cpu':80,'mem':120})
    worker2.setResource({'cpu':80,'mem':110})
#    master.restrk.addResource('w1',{'cpu':20,'mem':60})
#    master.restrk.addResource('w2',{'cpu':20,'mem':40})
#    master.restrk.addAgent({'agent':'j1','tasks':{'t1':{'last':100,'demands':{'cpu':2,'mem':8}}, 't2':{'last':100,'demands':{'cpu':2,'mem':8}}, 't3':{'last':10,'demands':{'cpu':2,'mem':8}} }})
#    master.restrk.addAgent({'agent':'j2','tasks':{'t1':{'last':100,'demands':{'cpu':1,'mem':32}}, 't2':{'last':100,'demands':{'cpu':1,'mem':32}}, 't3':{'last':10,'demands':{'cpu':1,'mem':32}} }})

    network.loadEvents([{'time':0,'node_id':'m','event':'start','msg':None},
                        {'time':0,'node_id':'w1','event':'start','msg':None},
                        {'time':0,'node_id':'w2','event':'start','msg':None},
                        {'time':30,'node_id':'m','event':'stop','msg':None},
                        {'time':30,'node_id':'w1','event':'stop','msg':None},
                        {'time':30,'node_id':'w2','event':'stop','msg':None}])
    agentnum = 10
    tasknum = 20
    agents = []
    for t in range(agentnum):
        cpu = random.randint(1,4)
        mem = random.randint(1,7)
        last = random.randint(10,100)
        j = {'agent':'j' + str(t),'tasks':dict()}
        for n in range(tasknum):
            j['tasks']['t'+str(n)] = {'last':last,'demands':{'cpu':cpu,'mem':mem}}
        agents.append({'time':t,'node_id':'m','event':'addagent','msg':j})
    network.loadEvents(agents)
    master.setExpectagentNum(agentnum)


##    agents = [{'time':0,'node_id':'m','event':'addagent','msg':None},
##            
##            ]

#    master.fillNode('w1')
#    master.fillNode('w2')
    network.addNode(master)
    network.addNode(worker1)
    network.addNode(worker2)

    network.run()

    
    #print master.restrk.wait_agents
    #print master.restrk.alloc_tasks
    #print master.restrk.alloc_resources
    #print master.restrk.resources
    #print master.restrk.node_resources
    #print '\n'

def ginidx(reses):
    m = float(sum(reses))/len(reses) if len(reses) > 0 else 0
    n = len(reses)
    g = sum([abs(i - j) for i in reses for j in reses])
    return g/(2*m*n**2) if m > 0 and n > 0 else 0

def variance(reses):
    m = float(sum(reses))/len(reses) if len(reses) > 0 else 0
    n = len(reses)
    return sum([(i - m)**2 for i in reses])/n if n > 0 else 0
    

def testRRR():
    stoptime = 50
    network = SimEnvironment()
    worker1 = Worker('w1',network,'m')
    worker2 = Worker('w2',network,'m')
    master = SchedMaster('m',network)
    
    worker1.setResource({'cpu':80,'mem':100})
    worker2.setResource({'cpu':80,'mem':100})

    network.loadEvents([{'time':0,'node_id':'m','event':'start','msg':None},
                        {'time':0,'node_id':'w1','event':'start','msg':None},
                        {'time':0,'node_id':'w2','event':'start','msg':None},
                        {'time':stoptime,'node_id':'m','event':'stop','msg':None},
                        {'time':stoptime,'node_id':'w1','event':'stop','msg':None},
                        {'time':stoptime,'node_id':'w2','event':'stop','msg':None}])
    f = open('exp1.input','r')

    agents = json.loads(f.readline())
    agentnum = len(agents)
    master.restrk.allagents = agentnum
    
                       
    f.close() 
    network.loadEvents(agents)
    master.setExpectagentNum(agentnum)

    network.addNode(master)
    network.addNode(worker1)
    network.addNode(worker2)

    network.run()

    #logdata:[ usage, envy, dom ] 
    fdom = open('domres.csv','w')
    fstat = open('dmstat.csv','w')
    #fres = open('othdata.csv','w')
    fstat.write('gini-coefficient,variance,maxsum,maxmin,envy_pairs,resource usages\n')
    ld = network.log.logd
    for t in range(stoptime):
        strfr = str(ld[t][1])
        strdm = str(ld[t][2][0])
        for r in ld[t][0]:
            strfr = strfr + ',' + str(r)
        
        for nd in range(1,len(ld[t][2])):
            strdm = strdm + ',' + str(ld[t][2][nd])
        dms = [dm for dm in ld[t][2] if dm > 0]
        fstat.write(str(ginidx(dms)) + ',' + str(variance(dms)) + ',' + str(sum(dms)) + ',' + str(min(dms)) + ',' +strfr + '\n')
        
        #fres.write(strfr + '\n')
        fdom.write(strdm + '\n')
    fdom.close()
    #fres.close()
    fstat.close()
            
        




def testResourceTracker():       

    rt = ResourceTracker()
    rt.addResource('w1',{'cpu':20,'mem':164})
    rt.addAgent({'agent':'j1','tasks':{'t1':{'last':100,'demands':{'cpu':2,'mem':8}}, 't2':{'last':100,'demands':{'cpu':2,'mem':8}}, 't3':{'last':10,'demands':{'cpu':2,'mem':8}} }})
    rt.addAgent({'agent':'j2','tasks':{'t1':{'last':100,'demands':{'cpu':1,'mem':32}}, 't2':{'last':100,'demands':{'cpu':1,'mem':32}}, 't3':{'last':10,'demands':{'cpu':1,'mem':32}} }})
    print rt.wait_agents
    print rt.alloc_tasks
    print rt.alloc_resources
    print rt.resources
    print rt.node_resources
    print '\n'

    rt.allocateTask('j2','t1',2,'w1')
    rt.allocateTask('j1','t1',2,'w1')
    rt.allocateTask('j1','t2',2,'w1')
    rt.allocateTask('j1','t3',2,'w1')
    print rt.wait_agents
    print rt.alloc_tasks
    print rt.alloc_resources
    print rt.resources
    print rt.node_resources
    print '\n'

    print rt.sortedDR()
    print '\n'

    rt.endTask('j1','t1')
    print rt.wait_agents
    print rt.alloc_tasks
    print rt.alloc_resources
    print rt.resources
    print rt.node_resources
    print '\n'

    print rt.dominateResource('j1')[0]

    print '\n'

    rt.endTask('j1','t2')
    rt.endTask('j1','t3')
    print rt.wait_agents
    print rt.alloc_tasks
    print rt.alloc_resources
    print rt.resources
    print rt.node_resources
    print '\n'

    rt.removeResource('w1',{'cpu':20,'mem':164})
    print rt.wait_agents
    print rt.alloc_tasks
    print rt.alloc_resources
    print rt.resources
    print rt.node_resources
    print '\n'                          


def testcase_gen():
    agentnum = 20
    tasknum = 20
    agents = []
    for t in range(agentnum):
        cpu = random.randint(1,4)
        mem = random.randint(1,7)
        last = random.randint(100,200)
        j = {'agent':'j' + str(t),'tasks':dict()}
        
        for n in range(tasknum):
            j['tasks']['t'+str(n)] = {'last':last,'demands':{'cpu':cpu,'mem':mem}}
        agents.append({'time':t,'node_id':'m','event':'addagent','msg':j})

    f = open('exp1.input','w')

    f.write(json.dumps(agents))
    f.close()
    

#testcase_gen()


testRRR()
#testResourceTracker()


        
