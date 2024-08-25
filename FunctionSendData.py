import gpiod
import serial
import time

TURN_LEFT = 24
TURN_RIGHT= 23
BUTTON = 18

#define variable

count = 0
flag = 0
position = 0
data_sendleft = False
data_sendright = False
data_sendstop = False
sethome = False
position_step = 0
chip = gpiod.Chip('/dev/gpiochip4')
#Setup turn left
turnleft = chip.get_line(TURN_LEFT)
turnleft.request(consumer='LEFT', type=gpiod.LINE_REQ_DIR_OUT)
#Setup turn right
turnright = chip.get_line(TURN_RIGHT)
turnright.request(consumer='RIGHT', type=gpiod.LINE_REQ_DIR_OUT)
#Setup button
button = chip.get_line(BUTTON)
button.request(consumer='BUTTON', type=gpiod.LINE_REQ_DIR_OUT)
# Cấu hình cổng serial
ser = serial.Serial(
    port='/dev/ttyAMA0', 
    baudrate=9600,      # BaudRate
    timeout=0
) 
    
def send_data(data):
    ser.write(data.encode())
    print(f"Send Data: {data}")

def receive_data():
    if ser.in_waiting > 0:
        data = ser.readline().decode('utf-8').rstrip()
        print(f"Receive Data: {data}")
        return data
    return None
#Start
def StartFunction():
    data = "StartFunction"
    send_data(data)
#Thinking
def ThinkingFunction():
    data = "ThinkingFunction"
    send_data(data)
#Listening
def ListeningFunction():
    data = "ListeningFunction"
    send_data(data)
#End
def EndFunction():
    data = "EndFunction"
    send_data(data)
#Answer
def AnswerFunction():
    data = "AnswerFunction"
    send_data(data)
#ThinkingDone
def ThinkingDoneFunction():
    data = "ThinkingDoneFunction"
    send_data(data)
#TurnLeftFunction   
def TurnLeftFunction():
    data = "TurnLeft"
    send_data(data)
#TurnRightFunction
def TurnRightFunction():
    data = "TurnRight"
    send_data(data)
def RainBowFunction():   
    data = "RainBow"
    send_data(data)

# def TurnLeft():
#     global data_sendstop
#     global position
#     print(f"TurnLeft")
#     if not data_sendstop:
#         TurnLeftFunction()
#         data_sendstop = True
#     turnleft.set_value(1)
#     turnright.set_value(0)

#     while True:
#         position += 1
#         if (button.get_value() == 1):  # Công tắc được chạm
#             position = 0
#             Stop()
                        
#             # Quay ngược lại cho đến khi công tắc không còn được chạm
#             while (button.get_value() == 1):
#                 turnleft.set_value(0)
#                 turnright.set_value(1)
#                 time.sleep(0.01)
            
#             # Dừng lại khi công tắc không còn được chạm
#             Stop()
#             break
#         print(f"Position: {position}")
    
#TurnRight
# def TurnRight():
#     global data_send
#     global position
    
#     print(f"TurnRight")
#     if not data_send:
#         TurnRightFunction()
#         data_send = True
        
#     turnright.set_value(1)
#     turnleft.set_value(0)

#     while True:
#         position -= 1
#         if (button.get_value() == 1):  # Công tắc được chạm
#             position = 0
#             Stop()
                        
#             # Quay ngược lại cho đến khi công tắc không còn được chạm
#             while (button.get_value() == 1):
#                 turnleft.set_value(1)
#                 turnright.set_value(0)
#                 time.sleep(0.01)
#             # Dừng lại khi công tắc không còn được chạm
#             Stop()
#             break
#         print(f"Position: {position}")

def Stop():

    turnright.set_value(0)
    turnleft.set_value(0)  


# def Position(countSet):
#     global position
#     global data_send
#     while(position != countSet):
#         if(position > countSet):
#             TurnRight()
#         if(position < countSet):
#             TurnLeft()
#         if(button.get_value() == 1):
#             turnleft.set_value(0)
#             turnright.set_value(0)
#     Stop()

# def Calib(positions):
#     global count
#     global flag
#     global sethome
#     global position
    
#     turnleft.set_value(1)
#     turnright.set_value(0)
#     time.sleep(0.3)
#     while True:
#         TurnRight()
#         if(button.get_value() == 1):
#             Stop()
#             position = 0          
#             flag = 1
#         if (flag == 1):   
#             Position(positions)
#             flag = 0
#             break

def TurnRight_OneStep():

    global position
    global button_dir

    turnright.set_value(1)
    turnleft.set_value(0)
    position -= 1
    button_dir = 1
    time.sleep(0.08)
    turnright.set_value(0)
    turnleft.set_value(0)    
    
    # Check the shared button signal
    if (button.get_value() == 1):  # Shared signal detected
        position = 0
        turnleft.set_value(0)
        turnright.set_value(0)
        
        # Rotate in the opposite direction until the signal is no longer detected
        
        while (button.get_value() == 1 ):
            turnleft.set_value(1)
            turnright.set_value(0)
            time.sleep(0.1)
            print("Outside buttonRight")
        
        # Stop the motors
        turnleft.set_value(0)
        turnright.set_value(0)
    
    print(f"Position: {position}")
          
    return position

def TurnLeft_OneStep():

    global position
    global button_dir

    turnright.set_value(0)
    turnleft.set_value(1)
    position += 1
    button_dir = 0
    time.sleep(0.08)
    turnright.set_value(0)
    turnleft.set_value(0)    
    
    # Check the shared button signal
    if (button.get_value() == 1):  # Shared signal detected
        position = 0
        turnright.set_value(0)
        turnleft.set_value(0)
                    
        # Rotate in the opposite direction until the signal is no longer detected

        while (button.get_value() == 1):
            turnleft.set_value(0)
            turnright.set_value(1)
            time.sleep(0.1)
            print("Outside buttonLeft")
        
        # Stop the motors
        turnright.set_value(0)
        turnleft.set_value(0)
    
    print(f"Position: {position}")
          
    return position
