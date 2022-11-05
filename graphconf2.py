import pandas as pd
import matplotlib.pyplot as plt
import plotly.graph_objects as go

df1 = pd.read_csv("10.csv")
size1=df1["Length"].tolist()
avg_size=0
joined_size=size1
for size in joined_size:
    avg_size=avg_size+size
avg_size=avg_size/len(joined_size)
time1=df1["Time"].tolist()
time=time1
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
    I=(r/(time[i]-check_time))*(avg_size/80000000)
    x.append(I)
    y.append(I*(avg_size/80000000)*(1-I))
    check_time=time[i]
    r=0



#fig = go.Figure(
    #data=[go.Line(x=x, y=y)],
    #layout=go.Layout(
        #title=go.layout.Title(text="Queuing Delay")
    #)
#)

#fig.show()
#print(y)
plt.plot(x,y)
plt.show()

