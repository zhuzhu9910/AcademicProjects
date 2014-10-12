
class MsgQueue:
    def __init__(self):
        self.msgqueue = list()

    def postMsg(self,msg):
        self.msgqueue.append(msg)

    def getMsg(self):
        if self.msgqueue:
            fmsg = self.msgqueue[0]
            del self.msgqueue[0]
            return fmsg
        return None


class Log:
    def __init__(self):
        self.time = 0
        self.logs = dict()

    def updateTime(self, t):
        self.time = t
    def log(self,log):
        if self.time in self.logs:
            self.logs[self.time].append(log)
        else:
            self.logs[self.time] = [log]
            
    def output(self,t = None):
        if t == None:
            t = self.time
        if t in self.logs:
            print 'time ' + str(t)
            for log in self.logs[t]:
                print log
            print '\n'
    def output_all(self):
        for t in self.logs:
            self.output(t)
    

class Node(object):
    def __init__(self,name,env):
        self.msgs = MsgQueue()
        self.env = env
        self.name = name #the id/name of the node
        self.running = False

    def setLog(self,log):
        self.log = log

    def step(self):
        pass

    def isStopped(self):
        return not self.running

    def setStop(self):
        self.running = False

    def setRun(self):
        self.running = True

    def execNodeEvent(self,event,msg):
        pass
    
    def postMsg(self,msg):
        self.msgs.postMsg(msg)


#simulation scheduler
class Simscheduler:
    def __init__(self):
        self.sched = list()
        
    def __iter__(self):
        return iter(self.sched)

    def sched_add(self,elm):
        self.sched.append(elm)

    def sched_clear(self):
        self.sched = list()

    def isEmpty(self):
        if self.sched:
            return False
        return True

class SimEnvironment:
    def __init__(self):
        self.schedule = Simscheduler()
        self.nodes = dict()   #nodes: {node_id, node object}
        self.time = 0
        self.events = dict()  #events: {time:[node_id,event]}
        self.log = Log()

    def init(self):
        self.resched()
        
    
    def finish(self):
        pass

    def resched(self):
        self.checkEvents()
        #print 'hi'
        try:
            for node in self.nodes:
                #print node.isStopped()
                if not self.nodes[node].isStopped():
                    self.schedule.sched_add(self.nodes[node])
        except:
            pass

    def checkEvents(self):
        try:
            for event in self.events[self.time]:
                self.nodes[event[0]].execNodeEvent(event[1],event[2])
        except:
            pass

    def step(self):
        self.before_step()
        for node in self.schedule:
            node.step()
        self.after_step()
        self.schedule.sched_clear()
        self.time = self.time + 1
        self.log.updateTime(self.time)
        self.resched()

    def before_step(self):
        pass

    def after_step(self):
        self.log.output()

    def isFinish(self):
        return not self.schedule.isEmpty()
    
    def run(self):
        self.init()
        while self.isFinish():           
            self.step()
        self.finish()

    def send(self,name,msg):
        try:
            self.nodes[name].postMsg(msg)
            return True
        except:
            return False

    def loadEvent(self, event):
  #      ev = [event['node_id'], event['event']]
        try:
            self.events[event['time']].append([event['node_id'], event['event'], event['msg']])
        except:
            self.events[event['time']] = [[event['node_id'], event['event'], event['msg']]]


    def loadEvents(self,events):
        for event in events:
            self.loadEvent(event)

    def addNode(self,node):
        self.nodes[node.name] = node
        node.setLog(self.log)
            
        
