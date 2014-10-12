
def DRF_Select(R,C,S,U,D):
    i = 0
    mins = 1
    for s in range(0,len(S)):
        if S[s] < mins:
            mins = S[s]
            i = s
    return i

def AssetF_Select(R,C,S,U,D):
    i = 0
    mins = 1
    for s in range(0,len(U)):
        if sum(U[s]) < mins:
            mins = sum(U[s])
            i = s
    return i


#R: total Resource capacity
#C: consumed resources
#S: users' dominate resources
#U: resources given to users
def drf_eq_tasks(R,C,S,U,D,selectUser):
    while True:
        i = selectUser(R,C,S,U,D)
        tC = list()
        tUi = list()
        for c in range(0,len(C)):
            if C[c] +D[i][c] > R[c]:
                return [C,U]
            tC.append(C[c] + D[i][c])
            tUi.append(U[i][c] + D[i][c])
        S[i] = max([ float(tUi[j])/R[j] for j in range(0,len(R))])
        C = tC
        U[i] = tUi

R = [100,500]
C = [0,0]
S = [0,0]
U = [[0,0],[0,0]]
D = [[3,3],[2,8]]

rst = drf_eq_tasks(R,C,S,U,D,DRF_Select)

print rst
        
