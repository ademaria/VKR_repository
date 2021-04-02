const humidityElement = document.querySelector('#humidity');
const temperatureElement = document.querySelector('#temperature');
const lastResponseElement = document.querySelector('#last-response-date');

try {
    socket = new WebSocket('ws://' + window.location.hostname + ':81/');
    socket.onmessage = (event) => {
        console.log({ event });
        // receive the data from the server
        const data = JSON.parse(event.data);
        console.log({ data });
        humidityElement.textContent = `${data.humidity} %`;
        temperatureElement.textContent = `${data.temperature}°C`;
        //Updates the last time data was recieved
        lastResponseElement.textContent = new Date();
    }
    console.log('Success start!');
    } catch(e) {
        console.error(e);
}