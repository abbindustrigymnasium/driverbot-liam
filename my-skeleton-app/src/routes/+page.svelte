<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>MQTT WEBSITE</title>
  <!-- Inkludera MQTT Bibliotek -->
  <script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.2/mqttws31.min.js"></script>
  <style>
    body {
      margin: 0;
      padding: 0;
      font-family: Arial, sans-serif;
      background-color: #f0f0f0;
    }

    /* Button style */
    button {  
      padding: 15px 25px;
      background-color: #4CAF50;
      color: white;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      margin: 5px;
      font-size: 16px;
      box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
      transition: background-color 0.3s ease;
    }
    /* Button hover effekt */
    button:hover {
      background-color: #0b3d09;
    }

    /* Button container */
    .button-container {
      position: absolute;
      display: flex;
      flex-direction: column;
      align-items: center;
      left: 850px;
      bottom: 100px;
      justify-content: center;
      height: calc(100vh - 100px); /* Justerad höjd */
      margin-top: 50px; /* Justerad margin */
    }

    /* Justeringar för platformar med mindre skärmar */
    @media screen and (max-width: 600px) {
      button {
        padding: 12px 20px;
        font-size: 14px;
      }
    }

    /* Style för container på vänster hörn */
    .bottom-left-buttons {
      position: absolute;
      bottom: 20px;
      left: 20px;
    }
  </style>
</head>
<body>
  <!-- Anslut till MQTT Button -->
  <button id="connectButton">Connect to MQTT</button>
  <!-- Button Container -->
  <div class="button-container">
    <button id="sendMessageButton1">W</button>
    <div>
      <button id="sendMessageButton4">A</button>
      <button id="sendMessageButton2">S</button>
      <button id="sendMessageButton5">D</button>
    </div>
    <button id="stopButton">Stop (Spacebar)</button>
  </div>

  <!-- Button Container för att starta inspelning och en uppspelningsknapp -->
  <div class="bottom-left-buttons">
    <button id="recordButton">Start Recording (R)</button>
    <button id="playButton">Start Playback</button>
  </div>

  <script>
    // Variabler för inspelning och uppspelning
    let isRecording = false;
    let isPlaying = false; // Tillagd
    let recordedCommands = [];
    let lastRecordTime = null;
    let currentPlaybackIndex = 0; // Tillagd

    // Inställningar för MQTT-Broker
    const brokerUrl = 'maqiatto.com';
    const port = 8883; // MQTT över WebSockets port
    const clientId = 'clientId_' + Math.random().toString(16).substr(2, 8); // Generera en slumpmässig klient-ID
    const username = 'liam.mardnas@hitachigymnasiet.se'; // Ditt Maqiatto-användarnamn
    const password = 'Fortnite'; // Ditt Maqiatto-lösenord

    // MQTT klientinstans
    let client = null;

    // Funktion för att ansluta till MQTT-Broker
    function connectToBroker() {
      // Initiera MQTT-klient
      client = new Paho.MQTT.Client(brokerUrl, port, clientId);

      // Sätt callback-hanterare
      client.onConnectionLost = onConnectionLost;
      client.onMessageArrived = onMessageArrived;

      // Anslut till MQTT-Broker
      client.connect({
        onSuccess: onConnect,
        userName: username,
        password: password,
        useSSL: false // Maqiatto stöder inte SSL
      });
    }

    // Funktion som körs när klienten ansluter
    function onConnect() {
      console.log('Ansluten till MQTT-Broker.');
    }

    // Funktion som körs när klienten förlorar sin anslutning
    function onConnectionLost(responseObject) {
      if (responseObject.errorCode !== 0) {
        console.error('Anslutning förlorad:', responseObject.errorMessage);
     

 }
    }

    // Funktion som körs när ett meddelande anländer
    function onMessageArrived(message) {
      console.log('Meddelande mottaget:', message.payloadString);
    }

    // Funktion för att skicka ett meddelande till broker
    function sendMessage(topic, message) {
      if (client && client.isConnected()) {
        const messageObject = new Paho.MQTT.Message(message);
        messageObject.destinationName = topic;
        client.send(messageObject);
        console.log('Meddelande skickat:', message);
      } else {
        console.error('Inte ansluten till MQTT-Broker.');
      }
    }

    // Funktion för att starta eller stoppa inspelning
    function toggleRecording() {
      isRecording = !isRecording;
      if (isRecording) {
        document.getElementById('recordButton').innerText = 'Stop Recording (R)';
        console.log('Inspelning startad.');
        recordedCommands = []; // Rensa tidigare inspelade kommandon
        lastRecordTime = new Date().getTime(); // Spela in starttid
      } else {
        document.getElementById('recordButton').innerText = 'Start Recording (R)';
        console.log('Inspelning stoppad.');
        console.log('Inspelade kommandon:', recordedCommands); // Visa inspelade kommandon
      }
    }

    // Funktion för att starta eller stoppa uppspelning
    async function togglePlayback() {
      isPlaying = !isPlaying;
      if (isPlaying) {
        document.getElementById('playButton').innerText = 'Stop Playback';
        console.log('Uppspelning startad.');
        currentPlaybackIndex = 0;
        await replicateCommands();
      } else {
        document.getElementById('playButton').innerText = 'Start Playback';
        console.log('Uppspelning stoppad.');
      }
    }

    // Funktion för att hantera inspelning av kommandon
    function recordCommand(topic, message) {
      if (isRecording) {
        const currentTime = new Date().getTime(); // Hämta aktuell tid
        const delay = currentTime - lastRecordTime; // Beräkna fördröjning sedan förra kommandot
        lastRecordTime = currentTime; // Uppdatera senast inspelade tid
        recordedCommands.push({ topic, message, delay });
        console.log('Kommando inspelat:', message, 'med fördröjning:', delay, 'ms');
      }
    }

    // Funktion för att replikera inspelade kommandon med fördröjning
    async function replicateCommands() {
      console.log('Replikerar inspelade kommandon...');
      for (const command of recordedCommands) {
        await sendMessageWithDelay(command.topic, command.message, command.delay); // Använd inspelad fördröjning
      }
    }

    // Funktion för att skicka ett meddelande till brokern med fördröjning beroende på tiden mellan knapptryck
    function sendMessageWithDelay(topic, message, delay) {
      return new Promise(resolve => {
        setTimeout(() => {
          sendMessage(topic, message);
          resolve();
        }, delay);
      });
    }

    // Händelselyssnare för knapptryckning för att växla inspelning
    document.getElementById('recordButton').addEventListener('click', toggleRecording);

    // Händelselyssnare för knapptryckning för att växla uppspelning
    document.getElementById('playButton').addEventListener('click', togglePlayback);

    // Händelselyssnare för knapptryckning för att ansluta
    document.getElementById('connectButton').addEventListener('click', () => {
      if (!client || !client.isConnected()) {
        connectToBroker();
      } else {
        console.log('Redan ansluten till MQTT-Broker.');
      }
    });

    // Händelselyssnare för tangentnedtryckning för att skicka meddelande 1 när "W" trycks
    document.addEventListener('keydown', (event) => {
      if (event.key === 'w' || event.key === 'W') {
        const topic = 'liam.mardnas@hitachigymnasiet.se/Motor'; // Ändra till önskat ämne
        const message = 'Fram'; // Ändra till önskat meddelande
        sendMessage(topic, message);
        recordCommand(topic, message);
      } else if (event.key === 's' || event.key === 'S') { // Om "S" trycks
        const topic = 'liam.mardnas@hitachigymnasiet.se/Motor'; // Ändra till önskat ämne
        const message = 'Bak'; // Ändra till önskat meddelande
        sendMessage(topic, message);
        recordCommand(topic, message);
      } else if (event.key === 'a' || event.key === 'A') { // Om "A" trycks
        const topic = 'liam.mardnas@hitachigymnasiet.se/Servo'; // Ändra till önskat ämne
        const message = 'Vänster'; // Ändra till önskat meddelande
        sendMessage(topic, message);
        recordCommand(topic, message);
      } else if (event.key === 'd' || event.key === 'D') { // Om "D" trycks
        const topic = 'liam.mardnas@hitachigymnasiet.se/Servo'; // Ändra till önskat ämne
        const message = 'Höger'; // Ändra till önskat meddelande
        sendMessage(topic, message);
        recordCommand(topic, message);
      } else if (event.key === 'r' || event.key === 'R') { // Om "R" trycks
        toggleRecording(); // Växla inspelning mellan start och stopp
      }
    });

    // Händelselyssnare för tangentnedtryckning för att skicka "Stop"-meddelande när mellanslag trycks på tangentbordet.
    document.addEventListener('keydown', (event) => {
      if (event.key === ' ') { // Om mellanslag trycks
        const topic = 'liam.mardnas@hitachigymnasiet.se/Motor'; // Ändra till önskad topic från maqiatto.com
        const message = 'Stop'; // Ändra till önskat meddelande
        sendMessage(topic, message);
        recordCommand(topic, message);
      }
    });
  </script>
</body>
</html>