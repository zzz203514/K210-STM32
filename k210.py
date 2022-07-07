import sensor, image, lcd, time
import KPU as kpu
import gc, sys
import ustruct

from fpioa_manager import fm
from machine import UART

fm.register(9, fm.fpioa.UART1_RX, force=True)
fm.register(10, fm.fpioa.UART1_TX, force=True)

fm.register(7, fm.fpioa.UART2_RX, force=True)
fm.register(8, fm.fpioa.UART2_TX, force=True)

uart1 = UART(UART.UART1, 115200, 8, 1, 0, timeout=1000, read_buf_len=4096)
uart2 = UART(UART.UART2, 115200, 8, 1, 0, timeout=1000, read_buf_len=4096)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)  #QVGA(320X240)
sensor.set_vflip(0) #后置模式，所见即所得
lcd.init()
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
sensor.skip_frames(time = 2000)

#分别为红绿蓝的颜色阙值
thresholds =[
            (30, 100, 15, 127, 15, 127),
            (0, 80, -70, -10, -0, 30),
            (0, 30, 0, 64, -128, -20),
            (53, 38, 127, -6, -6, 6)]

#1为红色，2为绿色，3为蓝色
white='0'
red='1'
green='2'
blue='3'
#传输识别出的二维码
def sending_qrcode(result):
    global uart2;
    data = ustruct.pack("<bbhhhb",
                   0x2C,
                   0X12,
                   int(result),
                   0x5B)
    uart2.write(data);

#传输坐标和颜色
def sending_data(color):
    global uart1;
    data = ustruct.pack("<bbhhhb",              #格式为俩个字符俩个短整型(2字节)
                   0x2C,                       #帧头1
                   0x12,                       #帧头2
                   int(color),              #数据是十六位的
                   0x5B)
    uart1.write(data);   #必须要传入一个字节数组

'''
def recive_data():
   global uart1,uart2
   if uart1.any() or uart2.any():
       tmp_data = uart1.readline();
       print(tmp_data)
'''

def find_max(blobs):    #定义寻找色块面积最大的函数
   max_size=0
   global max_blob
   for blob in blobs:
       if blob.pixels() > max_size:
           max_blob=blob
           max_size = blob.pixels()
   return max_blob

while(True):
    img = sensor.snapshot()
    res = img.find_qrcodes()
    if len(res) > 0:
        result =""
        img.draw_string(2,2,res[0].payload(), color=(0,128,0),scale=2)
        result=res[0].payload()
        sending_qrcode(result)
        #recive_data();
    for blob in img.find_blobs(thresholds, pixels_threshold=500, area_threshold=1000, merge=True):
        cx=0;cy=0;
        max_blob = find_max(img.find_blobs(thresholds, pixels_threshold=500, area_threshold=1000, merge=True));
        if max_blob.code() == 1:
            img.draw_rectangle(max_blob[0:4])
            img.draw_cross(max_blob[5],max_blob[6])
            img.draw_string(blob.x() + 2, blob.y() + 2, "red")
            cx=max_blob[5];
            cy=max_blob[6];
            sending_data(red)       #发送点位坐标
            #recive_data();
        if max_blob.code() == 2:
            img.draw_rectangle(max_blob[0:4])
            img.draw_cross(max_blob[5],max_blob[6])
            img.draw_string(blob.x() + 2, blob.y() + 2, "green")
            cx=max_blob[5];
            cy=max_blob[6];
            sending_data(green)       #发送点位坐标
            #recive_data();
        if max_blob.code() == 4:
            img.draw_rectangle(max_blob[0:4])
            img.draw_cross(max_blob[5],max_blob[6])
            img.draw_string(blob.x() + 2, blob.y() + 2, "blue")
            cx=max_blob[5];
            cy=max_blob[6];
            sending_data(blue)       #发送点位坐标
            #recive_data();
        if max_blob.code()==8:
            img.draw_rectangle(max_blob[0:4])
            img.draw_cross(max_blob[5],max_blob[6])
            img.draw_string(blob.x() + 2, blob.y() + 2, "white")
            sending_qrcode(white)
    lcd.rotation(2)
    lcd.display(img)

