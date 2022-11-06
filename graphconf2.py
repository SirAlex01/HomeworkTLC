import pandas as pd
import matplotlib.pyplot as plt
import plotly.graph_objects as go

df1 = pd.read_csv("conf2.csv")
size1=df1["Length"].tolist()
avg_size=0
joined_size=size1
for size in joined_size:
    avg_size=avg_size+size
avg_size=avg_size/len(joined_size)
time=df1["Time"].tolist()
x=[]
y=[]
check_time=time[0]
r=0
for i in range(0,len(joined_size)):
    r=r+(size1[i]/avg_size)
    if time[i]-check_time<=0.3:
        continue
    #print((r/(time[i]-check_time))*(avg_size/80000000),r,time[i])
    I=(r/(time[i]-check_time))*(avg_size/83886080)
    x.append(I)
    y.append(I*(avg_size/83886080)*(1-I))
    check_time=time[i]
    r=0

#print(x,y)

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

