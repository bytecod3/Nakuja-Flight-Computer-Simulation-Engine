k = [1,2,3,4,5,6,7,8,9,10,23,18,17,6,5,4,3,2,0]
mx = 0
mn = 0

apg_flag = 0
apg_val = 0;

for i in range(0, len(k)):
    if k[i] > mx:
        mx = k[i]
    if apg_flag == 0:
        if (mx - k[i]) > 3:
            apg_val = mx
            apg_flag = 1 
            print("Apogee achieved at " + str(apg_val)) 
