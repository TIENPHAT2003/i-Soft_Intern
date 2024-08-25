from picamera2 import Picamera2
from libcamera import Transform
from FunctionSendData import*
import cv2
import mediapipe as mp
import math
import threading

# Khởi tạo MediaPipe Face Detection
mp_face_detection = mp.solutions.face_detection
mp_drawing = mp.solutions.drawing_utils

face_detection = mp_face_detection.FaceDetection(model_selection=1, min_detection_confidence=0.7)

# Khởi tạo PiCamera2
camera = Picamera2()
sensor_modes = camera.sensor_modes
config = camera.create_preview_configuration(transform=Transform(vflip=True, hflip=True))
camera.configure(config)
camera.start()

# Task read button
def read_button():
    global button_pressed
    global check
    while True:
        if button.get_value() == 1:
            button_pressed = True
            check = True
        else:
            button_pressed = False
        time.sleep(0.01) 

button_thread = threading.Thread(target=read_button)
button_thread.daemon = True
button_thread.start()
#End task read button

# Task SendData
data_sendleft = False
data_sendright = False
data_sendstop = False

def check_SendData():
    global dir, data_sendleft, data_sendright, data_sendstop
    while True:
        if dir == 1 and not data_sendright:
            data_sendstop = data_sendleft = False
            print("Right Scanning")
            TurnRightFunction()
            data_sendright = True

        elif dir == 0 and not data_sendleft:
            data_sendright = data_sendstop = False
            print("Left Scanning")
            TurnLeftFunction()
            data_sendleft = True  

        elif dir == -1 and not data_sendstop:
            data_sendleft = data_sendright = False  
            RainBowFunction()
            data_sendstop = True
        time.sleep(0.01)
                
sendData_thread = threading.Thread(target=check_SendData)
sendData_thread.daemon = True
sendData_thread.start()
#End task SendData


# Main loop to process camera input
try:
    scan_start_time = None
    check = False 
    checkscan = False
    scanning = False
    dir = -1
    while True:
      
        # Lấy khung hình từ PiCamera
        frame = camera.capture_array()
        frame = cv2.rotate(frame, cv2.ROTATE_180)
        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

        # Xử lý phát hiện khuôn mặt
        results = face_detection.process(frame_rgb)
        if button_pressed:
            scanning = True
        else:
            
            if results.detections:
                scanning = False
                checkscan = False
                closest_face = None
                min_distance_to_center = float('inf')
                h, w, _ = frame_rgb.shape
                center_x = w // 2
                center_y = h // 2
                for detection in results.detections:
                    bbox = detection.location_data.relative_bounding_box
                    face_x = int((bbox.xmin + bbox.width / 2) * w)
                    face_y = int((bbox.ymin + bbox.height / 2) * h)

                    distance_to_center = math.sqrt((face_x - center_x) ** 2 + (face_y - center_y) ** 2)

                    if distance_to_center < min_distance_to_center:
                        min_distance_to_center = distance_to_center
                        closest_face = detection

                if closest_face:

                    nose = closest_face.location_data.relative_keypoints[2]

                    nose_x = int(nose.x * w)
                    nose_y = int(nose.y * h)

                    
                    distance = nose_x - w / 2

                    cv2.circle(frame_rgb, (nose_x, nose_y), 10, (255, 130, 0), -1)

                    if distance < -80:  
                        TurnLeft_OneStep()
                    elif distance > 80:  
                        TurnRight_OneStep()
                    elif distance >= -80 and distance <= 80:
                        dir = -1
                        Stop()
                    
            else:
                if not checkscan:
                    print("Start Scanning")
                    scan_start_time = time.monotonic()
                    checkscan = True
                    
                elif scan_start_time and time.monotonic() - scan_start_time >= 10:
                    dir = 1
                    scanning = True
                    checkscan = False
                    scan_start_time = None 
                    

        if scanning:
            
            if dir == 1 :
                TurnRight_OneStep()
                time.sleep(0.05)
                
            else:
                TurnLeft_OneStep()
                time.sleep(0.05)
                
            if not button_pressed:
                if check:
                    dir = not dir
                    check = False 
                            
        # Hiển thị khung hình với phát hiện khuôn mặt
        
        cv2.imshow('Face Detection', frame_rgb)

        # Nhấn 'q' để thoát
        if cv2.waitKey(1) & 0xFF == ord('q'):
            EndFunction()
            break

except KeyboardInterrupt:
    Stop()
    EndFunction()
    camera.stop()
    cv2.destroyAllWindows()

