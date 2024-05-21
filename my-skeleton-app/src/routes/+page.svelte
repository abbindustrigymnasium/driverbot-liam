<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>MQTT Button</title>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.2/mqttws31.min.js"></script>
  <style>
    /* Button styling */
    button {
      padding: 20px 30px;
      background-color: #4CAF50;
      color: white;
      border: none;
      border-radius: 8px;
      cursor: pointer;
      margin: 5px;
      font-size: 18px;
    }

    /* Change button color on hover */
    button:hover {
      background-color: #45a049;
    }

    /* Container for buttons */
    .button-container {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: flex-start;
      height: 80vh; /* Adjusted height */
      margin-top: 20vh; /* Adjusted margin-top */
    }
  </style>
</head>
<body>
  <button id="connectButton">Connect to MQTT</button>
  <div class="button-container">
    <button id="sendMessageButton1">W</button>
    <div>
      <button id="sendMessageButton4">A</button>
      <button id="sendMessageButton2">S</button>
      <button id="sendMessageButton5">D</button>
    </div>
    <button id="stopButton">Stop</button>
  </div>
 
  <script>
    // MQTT broker connection options
    const brokerUrl = 'maqiatto.com';
    const port = 8883; // MQTT over WebSockets port
    const clientId = 'clientId_' + Math.random().toString(16).substr(2, 8); // Generate a random client ID
    const username = 'liam.mardnas@hitachigymnasiet.se'; // Your Maqiatto username
    const password = 'Fortnite'; // Your Maqiatto password
 
    // MQTT client instance
    let client = null;
 
    // Function to connect to the MQTT broker
    function connectToBroker() {
      // Initialize MQTT client
      client = new Paho.MQTT.Client(brokerUrl, port, clientId);
     
      // Set callback handlers
      client.onConnectionLost = onConnectionLost;
      client.onMessageArrived = onMessageArrived;
 
      // Connect to MQTT broker
      client.connect({
        onSuccess: onConnect,
        userName: username,
        password: password,
        useSSL: false // Maqiatto does not support SSL
      });
    }
 
    // Called when the client connects
    function onConnect() {
      console.log('Connected to MQTT broker.');
    }
 
    // Called when the client loses its connection
    function onConnectionLost(responseObject) {
      if (responseObject.errorCode !== 0) {
        console.error('Connection lost:', responseObject.errorMessage);
      }
    }
 
    // Called when a message arrives
    function onMessageArrived(message) {
      console.log('Message received:', message.payloadString);
    }
 
    // Function to send a message to the broker
    function sendMessage(topic, message) {
      if (client && client.isConnected()) {
        const messageObject = new Paho.MQTT.Message(message);
        messageObject.destinationName = topic;
        client.send(messageObject);
        console.log('Message sent:', message);
      } else {
        console.error('Not connected to MQTT broker.');
      }
    }
 
    // Event listener for button click to connect
    document.getElementById('connectButton').addEventListener('click', () => {
      if (!client || !client.isConnected()) {
        connectToBroker();
      } else {
        console.log('Already connected to MQTT broker.');
      }
    });
 
    // Event listener for button click to send message 1
    document.getElementById('sendMessageButton1').addEventListener('click', () => {
      const topic = 'liam.mardnas@hitachigymnasiet.se/Motor'; // Change to your desired topic
      const message = 'Fram'; // Change to your desired message
      sendMessage(topic, message);
    });

    // Event listener for button click to send message 2
    document.getElementById('sendMessageButton2').addEventListener('click', () => {
      const topic = 'liam.mardnas@hitachigymnasiet.se/Motor'; // Change to your desired topic
      const message = 'Bak'; // Change to your desired message
      sendMessage(topic, message);
    });
    document.getElementById('sendMessageButton4').addEventListener('click', () => {
      const topic = 'liam.mardnas@hitachigymnasiet.se/Servo'; // Change to your desired topic
      const message = 'Vänster'; // Change to your desired message
      sendMessage(topic, message);
    });
    document.getElementById('sendMessageButton5').addEventListener('click', () => {
      const topic = 'liam.mardnas@hitachigymnasiet.se/Servo'; // Change to your desired topic
      const message = 'Höger'; // Change to your desired message
      sendMessage(topic, message);
    });

    // Event listener for keydown event to send message 1 when "W" key is pressed
    document.addEventListener('keydown', (event) => {
      if (event.key === 'w' || event.key === 'W') {
        const topic = 'liam.mardnas@hitachigymnasiet.se/Motor'; // Change to your desired topic
        const message = 'Fram'; // Change to your desired message
        sendMessage(topic, message);
      } else if (event.key === 's' || event.key === 'S') { // If "S" key is pressed
        const topic = 'liam.mardnas@hitachigymnasiet.se/Motor'; // Change to your desired topic
        const message = 'Bak'; // Change to your desired message
        sendMessage(topic, message);
      } else if (event.key === 'a' || event.key === 'A') { // If "A" key is pressed
        const topic = 'liam.mardnas@hitachigymnasiet.se/Servo'; // Change to your desired topic
        const message = 'Vänster'; // Change to your desired message
        sendMessage(topic, message);
      } else if (event.key === 'd' || event.key === 'D') { // If "D" key is pressed
        const topic = 'liam.mardnas@hitachigymnasiet.se/Servo'; // Change to your desired topic
        const message = 'Höger'; // Change to your desired message
        sendMessage(topic, message);
      }
    });

    // Event listener for button click to send "Stop" message
    document.getElementById('stopButton').addEventListener('click', () => {
      const topic = 'liam.mardnas@hitachigymnasiet.se/Motor'; // Change to your desired topic
      const message = 'Stop'; // Change to your desired message
      sendMessage(topic, message);
    });

    // Event listener for keydown event to send "Stop" message when spacebar is pressed
    document.addEventListener('keydown', (event) => {
      if (event.key === ' ') { // If spacebar is pressed
        const topic = 'liam.mardnas@hitachigymnasiet.se/Motor'; // Change to your desired topic
        const message = 'Stop'; // Change to your desired message
        sendMessage(topic, message);
      }
    });
  </script>
</body>
</html>
