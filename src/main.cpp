#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include "ESPAsyncWebServer.h"
#include <GDBStub.h>
#include "motor_control.h"

// 设置AP模式的SSID和密码
const char *ssid = "Jerry_智能小车_AP2"; // 自定义ESP8266热点的名称
const char *password = "12345678";       // 热点的密码

// 固定的IP地址配置
IPAddress local_IP(192, 168, 10, 1); // 你想要设置的固定IP地址
IPAddress gateway(192, 168, 10, 1);  // 默认网关（可以设置为与ESP的IP相同）
IPAddress subnet(255, 255, 255, 0);  // 子网掩码

const byte DNS_PORT = 53; // DNS端口号
DNSServer dnsServer;      // 创建dnsServer实例  开启强制门户

int motorSpeed = 50; //   默认速度  50%
int speed = 1023 * (50 / 100.0);

bool isMovingForward = false;   // 是否正在前进的状态标识
int lastLeftSensorValue = HIGH; // 用于保存上次的传感器状态
int lastRightSensorValue = HIGH;

// generateHTML控制网页内容
String generateHTML = R"(
  <html>
     <head>
          <meta charset="UTF-8">
          <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no"> <!-- 禁止缩放 -->
          <style>
            body {
              font-family: Arial, sans-serif;
              text-align: center;
              margin: 0;
              padding: 0;
              background-color: #f4f4f4; 
              user-select: none; 
              -webkit-user-select: none; 
              -moz-user-select: none; 
              touch-action: manipulation; 
            }
            .button-container {
              margin: 16px 16px;
            }
            button {
              font-size: 20px;
              padding: 10px 20px;
              margin: 5px;
              background-color: #4CAF50;
              color: white;
              border: none;
              border-radius: 8px; 
              cursor: pointer;
              width: 76%; 
              box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); 
              transition: background-color 0.3s, transform 0.3s; 

            }
            button.active {
               transform: scale(1.2); 
            }
            button:hover {
              background-color: #45a049;
            }
            .slider-container {
              margin: 16px 16px;
            }
            .slider-label {
              font-size: 18px;
              margin-bottom: 10px;
              display: block;
            }
            .slider {
              -webkit-appearance: none;
              appearance: none;
              width: 76%;
              height: 30px;
              background-color: #4CAF50;
              border-radius: 8px;
              outline: none;
              transition: background-color 0.3s;
              box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
            }
            .slider::-webkit-slider-thumb {
              -webkit-appearance: none;
              appearance: none;
              width: 30px;
              height: 30px;
              background-color: white;
              border-radius: 50%;
              cursor: pointer;
              box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
              transition: transform 0.3s;
            }
            .slider:active::-webkit-slider-thumb {
              transform: scale(1.2);
            }
            .slider::-moz-range-thumb {
              width: 30px;
              height: 30px;
              background-color: white;
              border-radius: 50%;
              cursor: pointer;
              box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
              transition: transform 0.3s;
            }
            .slider:active::-moz-range-thumb {
              transform: scale(1.2);
            }
            @media (min-width: 600px) { 
              button {
                width: auto; 
              }
            }
          </style>
          <script>
            function sendCommand(command) {
              fetch('/' + command)
              .then(response => response.text())
              .then(data => console.log(data))
              .catch(error => console.error('Error:', error));
            }
            function handleTouchStart(event, command) {
              event.target.classList.add('active'); // 按下时添加放大效果
              sendCommand(command);
            }

            function handleTouchEnd(event) {
              event.target.classList.remove('active'); // 松开后移除放大效果
              sendCommand('stop');
            }
            function updateSpeed(value) {
              fetch('/setSpeed?speed=' + value)
                .then(response => response.text())
                .then(data => console.log(data))
                .catch(error => console.error('Error:', error));
            }

            
          </script>
        </head> 
        <body>
          <h1>Jerry Motor Control</h1>
          <div class="button-container">
            <button 
              ontouchstart="handleTouchStart(event,'forward');"
              ontouchend="handleTouchEnd(event);">前进(Forward)
            </button>
          </div>
      
          <div class="button-container">
            <button 
              ontouchstart="handleTouchStart(event,'left');" 
              ontouchend="handleTouchEnd(event);">左转(Left)
            </button>
            <button 
              ontouchstart="handleTouchStart(event,'right');" 
              ontouchend="handleTouchEnd(event);">右转(Right)
            </button>
          </div>
      
          <div class="button-container">
            <button 
              ontouchstart="handleTouchStart(event,'backward');" 
              ontouchend="handleTouchEnd(event);">后退(Backward)
            </button>
          </div>

          <div class="button-container">
            <button 
              ontouchstart="handleTouchStart(event,'stop');" 
              <!--ontouchend="handleTouchEnd(event);">停止(Stop)-->
            </button>
          </div>
          <div class="slider-container">
            <label class="slider-label" for="speedSlider">速度(Speed)</label>
            <input type="range" min="0" max="100" value="50" class="slider" id="speedSlider" oninput="updateSpeed(this.value);" </input>
          </div>
        </body>
      </html>
    )";

// 初始化DNS服务器
void initDNS()
{
  if (dnsServer.start(DNS_PORT, "*", local_IP))
  {
    // 判断将所有地址映射到esp8266的ip上是否成功
    Serial.println("start dnsserver success.");
  }
  else
  {
    Serial.println("start dnsserver failed.");
  }
}

// 障碍物检测和电机控制逻辑
void detectObstacleAndMove()
{
  int leftSensorValue = readLeftSensorValue();   // 读取左侧传感器值
  int rightSensorValue = readRightSensorValue(); // 读取右侧传感器值

  // 打印传感器的值，检查是否正确工作
  Serial.println();
  Serial.print("Left Sensor: ");
  Serial.println(leftSensorValue);
  Serial.print("Right Sensor: ");
  Serial.println(rightSensorValue);

  Serial.print("LastLeft Sensor: ");
  Serial.println(lastLeftSensorValue);
  Serial.print("LastRight Sensor: ");
  Serial.println(lastRightSensorValue);

  if (leftSensorValue == LOW && rightSensorValue == LOW)
  {
    if (lastLeftSensorValue != LOW || lastRightSensorValue != LOW)
    {
      // 如果两个传感器都检测到障碍物，停止
      stopMotor();
      Serial.println("前方有障碍物，停止");
    }
    else
    {
      Serial.println("前方无障碍物，继续");
    }
  }
  else if (leftSensorValue == LOW)
  {
    if (lastLeftSensorValue != LOW)
    {
      // 如果左传感器检测到障碍物，右转
      turnRight();
      Serial.println("左侧有障碍物，右转");
    }
    else
    {
      Serial.println("左侧无障碍物，继续");
    }
  }
  else if (rightSensorValue == LOW)
  {
    if (lastRightSensorValue != LOW)
    {
      // 如果右传感器检测到障碍物，左转
      turnLeft();
      Serial.println("右侧有障碍物，左转");
    }
    else
    {
      Serial.println("右侧无障碍物，继续");
    }
  }
  else
  {
    if (lastLeftSensorValue != HIGH || lastRightSensorValue != HIGH)
    {
      // 没有障碍物，继续前进
      moveForward();
      Serial.println("last前进中，无障碍");
    }
    else
    {
      Serial.println("前进中，无障碍");
    }
  }

  // 更新上次的传感器状态
  lastLeftSensorValue = leftSensorValue;
  lastRightSensorValue = rightSensorValue;
}

void setup()
{

  Serial.begin(115200);
  Serial.println("Program started");
  // 初始化GDB调试
  // gdbstub_init();

  // 设置L298n驱动引脚
  setupMotorPins();
  // 初始化红外传感器引脚
  setupMSensorPins();
  // 设置固定IP地址
  if (!WiFi.softAPConfig(local_IP, gateway, subnet))
  {
    Serial.println("Failed to configure AP");
  }

  // 启动AP模式
  WiFi.softAP(ssid, password);
  // WiFi.softAP(ssid);
  Serial.println();
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP()); // 打印 ESP8266 AP模式下的 IP 地址
  // dnsServer.start(DNS_PORT, "*", local_IP);
  initDNS(); // 启动dnsServer

  // 设置网页内容
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              Serial.println();
              Serial.print("打开首页: ");
              //request->send(200, "text/html", generateHTML); 
              AsyncWebServerResponse *response = request->beginResponse(200, "text/html; charset=utf-8", generateHTML);
              request->send(response); });

  // 捕获所有未处理请求
  server.onNotFound([](AsyncWebServerRequest *request)
                    {
    // 只重定向非电机控制请求到根目录
    if (!request->url().startsWith("/forward") &&
        !request->url().startsWith("/backward") &&
        !request->url().startsWith("/left") &&
        !request->url().startsWith("/right") &&
        !request->url().startsWith("/stop")&&
        !request->url().startsWith("/setSpeed")) {
            Serial.println();
            Serial.print("非内部请求，强制跳转门户: ");
            AsyncWebServerResponse *response = request->beginResponse(200, "text/html; charset=utf-8", generateHTML);
              request->send(response);  //非内部请求，强制跳转门户
    } else {
      // 对于电机控制请求，返回 404 或适当响应
      //request->send(404, "text/plain", "Not Found");
    } });

  // 创建Web接口来控制电机
  server.on("/forward", AsyncWeb_HTTP_GET, [](AsyncWebServerRequest *request)
            {
    isMovingForward = true;
    moveForward(speed);
    request->send(200, "text/plain", "前进(Moving Forward)"); });

  server.on("/backward", AsyncWeb_HTTP_GET, [](AsyncWebServerRequest *request)
            {
    moveBackward(speed);
    request->send(200, "text/plain", "后退(Moving Backward)"); });

  server.on("/left", AsyncWeb_HTTP_GET, [](AsyncWebServerRequest *request)
            {
    turnLeft(speed);
    request->send(200, "text/plain", "左转(Turning Left)"); });

  server.on("/right", AsyncWeb_HTTP_GET, [](AsyncWebServerRequest *request)
            {
    turnRight(speed);
    request->send(200, "text/plain", "右转(Turning Right)"); });

  server.on("/stop", AsyncWeb_HTTP_GET, [](AsyncWebServerRequest *request)
            {
    isMovingForward = false; 
    stopMotor();
    request->send(200, "text/plain", "停止(Stopping)"); });

  // 处理电机调速请求
  server.on("/setSpeed", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam("speed")) {
      motorSpeed = request->getParam("speed")->value().toInt();
      speed = 1023 * (motorSpeed/100.0);
      Serial.println();
      Serial.print("调速请求: ");
      Serial.print(motorSpeed);
      Serial.print("-->>> ");
      Serial.print(speed);
      request->send(200, "text/plain", "Speed set to: " + String(motorSpeed));
    } else {
      request->send(400, "text/plain", "No speed parameter provided");
    } });

  // 启动Web服务器
  server.begin();
  Serial.println("Web服务器已启动");
  moveForward();
  delay(100);
  moveBackward();
  delay(100);
  turnLeft();
  delay(100);
  turnRight();
  delay(100);
  stopMotor();
}

void loop()
{
  // 处理DNS请求
  dnsServer.processNextRequest();

  if (isMovingForward)
  {
    Serial.println();
    Serial.print("启动红外障碍: ");
    Serial.print(isMovingForward);
    detectObstacleAndMove();
    delay(100); // 延时，避免频繁检测
  }
}
