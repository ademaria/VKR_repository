char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP SERVER</title>
  <style>
    @import url(https://fonts.googleapis.com/css?family=Rubik+One&subset=latin,cyrillic);

    * {
      margin: 0;
      padding: 0;
    }

    .root {
      padding: 40px 20px;
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    .data-list {
      margin: 20px 0;
      display: flex;
      flex-direction: column;
      font-size: 20px;
    }

    .data-item {
      display: flex;
      margin-bottom: 10px;
    }

    .item-name {
      margin-right: 10px;
    }

    .last-response {
      margin-right: 5px;
    }
    .item-name::after, .last-response::after {
      content: ':';
    }

    .item-value {
      color: #0066cc;
      font-family: 'Rubik One', sans-serif;
    }

    #temperature {
      background: linear-gradient(45deg, #1e50a0 33%, #AF3203 66%, #FFA533);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
    }

    .last-response {
      font-size: 16px;
    }
  </style>
</head>

<body>
  <div class="root">
    <h1 class="header">ESP SERVER</h1>

    <div class="data-list">
      <div class="data-item">
        <div class="item-name">humidity</div>
        <div class="item-value" id="humidity">---</div>
      </div>
      <div class="data-item">
        <div class="item-name">temperature</div>
        <div class="item-value" id="temperature">---</div>
      </div>
    </div>

    <div>
      <span class="last-response">
        Последний ответ было получен
      </span>
      <span id="last-response-date">
        никогда
      </span>
    </div>
</div>
  <script>
    const humidityElement = document.querySelector('#humidity');
    const temperatureElement = document.querySelector('#temperature');
    const lastResponseElement = document.querySelector('#last-response-date');

    try {
      socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      socket.onmessage = (event) => {
        console.log({ event });
        // receive the color data from the server
        const data = JSON.parse(event.data);
        console.log({ data });
        humidityElement.textContent = `${data.humidity} %`;
        temperatureElement.textContent = `${data.temperature}°C`;
        lastResponseElement.textContent = new Date();
      }
      console.log('Success start!');
    } catch(e) {
      console.error(e);
    }
  </script>
</body>
</html>
)=====";
