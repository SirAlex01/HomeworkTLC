import pandas as pd
import matplotlib.pyplot as plt

df1 = pd.read_csv("10.csv")
df1=df1.drop(labels=[0,1,1247,1248],axis=0)
df2 = pd.read_csv("192.csv")
df2=df2.drop(labels=[0,1,866,867],axis=0)
size1=df1["Length"].tolist()
size2=df2["Length"].tolist()
avg_size=0
joined_size=size1+size2
for size in joined_size:
    avg_size=avg_size+size
avg_size=avg_size/len(joined_size)
time1=df1["Time"].tolist()
time2=df2["Time"].tolist()
time=time1+time2
time.sort()
x=[]
y=[]
time_0=time[0]
check_time=time[0]
r=0
for i in range(1,len(joined_size)):
    r=r+1
    if time[i]-check_time<=0.25:
        continue
    #print((r/(time[i]-check_time))*(avg_size/80000000),r,time[i])
    x.append(time[i])
    y.append((r/(time[i]-check_time))*(avg_size/80000000))
    check_time=time[i]
    r=0

plt.plot(x,y)
plt.show()
