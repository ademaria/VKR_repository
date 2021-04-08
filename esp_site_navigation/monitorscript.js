const IDElement = document.querySelector('#ID');
const temperatureElement = document.querySelector('#temperature');
const humidityElement = document.querySelector('#humidity');
const ambientLightElement = document.querySelector('#ambientLight');
const lastResponseElement = document.querySelector('#last-response-date');

try {
    socket = new WebSocket('ws://' + window.location.hostname + ':81/');
    socket.onmessage = (event) => {
        console.log({ event });
        // receive the data from the server
        const data = JSON.parse(event.data);
        console.log({ data });
        IDElement.textContent = `${data.ID}`;
        temperatureElement.textContent = `${data.temperature}°C`;
        humidityElement.textContent = `${data.humidity} %`;
        ambientLightElement.textContent = `${data.ambientLight} %`;
        //Updates the last time data was recieved
        lastResponseElement.textContent = new Date();
    }
    console.log('Success start!');
    } catch(e) {
        console.error(e);
}