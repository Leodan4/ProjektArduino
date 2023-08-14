#ifndef WebLED_H
#define WebLED_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #fc0307;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 42px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    user-select: none;
   }
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
    #list {
     display: flex;
     justify-content: center;
     padding:10px;
     gap:10px;
  }
  @media (max-width: 770px){
    .card{
      padding-top :5px;
      padding-bottom :10px;
    }
    .button{
      padding: 15px 25px;
    }
    .state{
      margin-top: 16px;
      margin-bottom: 16px;
    }
    #list{
      display: block;
      padding-top: 0px;
      padding-bottom: 0px;
    }
  }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>Pelatihan IoT Moklet</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Kendali Lampu IoT</h2>
      <p class="state">Status : <span id="state">ON</span></p>
      <p><button id="toggle" onclick="toggle()" class="button">Lampu Node</button></p>
      <div id="list">
        <p class="state">LED M : <span id="led1_id">OFF</span></p>
        <p class="state">LED K : <span id="led2_id">OFF</span></p>
        <p class="state">LED H : <span id="led3_id">OFF</span></p>
      </div>
      <div id="list">
        <p><button id="toggle" onclick="led1()" class="button">LED M</button></p>
        <p><button id="toggle" onclick="led2()" class="button">LED K</button></p>
        <p><button id="toggle" onclick="led3()" class="button">LED H</button></p>
      </div>
    </div>
  </div>
<script>
  var state="OFF",status,state1, state2, state3;
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', reload);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    status=event.data;
    state=parseInt(status[0]) ? "OFF" : "ON";
    led1_status=parseInt(status[2]) ? "ON" : "OFF";
    led2_status=parseInt(status[4]) ? "ON" : "OFF";
    led3_status=parseInt(status[6]) ? "ON" : "OFF";

    document.getElementById('state').innerHTML = state;
    document.getElementById('led1_id').innerHTML = led1_status;
    document.getElementById('led2_id').innerHTML = led2_status;
    document.getElementById('led3_id').innerHTML = led3_status;
  }

  function reload(){
    initWebSocket();
    websocket.send('status');
  }
  function toggle(){
    websocket.send('toggle');
  }
  function led1(){
    websocket.send('led1');
  }
  function led2(){
    websocket.send('led2');
  }
  function led3(){
    websocket.send('led3');
  }
</script>
</body>
</html>
)rawliteral";

#endif // WebLED_H