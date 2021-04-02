var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    websocket.send("states");
}
  
function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
} 

function onMessage(event) {
    var myObj = JSON.parse(event.data);
            console.log(myObj);
            for (i in myObj.gpios){
                var output = myObj.gpios[i].output;
                var state = myObj.gpios[i].state;
                console.log(output);
                console.log(state);
                if (state == "1"){
                    document.getElementById(output).checked = true;
                    document.getElementById(output+"s").innerHTML = "ON";
                }
                else{
                    document.getElementById(output).checked = false;
                    document.getElementById(output+"s").innerHTML = "OFF";
                }
            }
    console.log(event.data);
}

// Send Requests to Control GPIOs
function toggleCheckbox (element) {
    console.log(element.id);
    websocket.send(element.id);
    if (element.checked){
        document.getElementById(element.id+"s").innerHTML = "ON";
    }
    else {
        document.getElementById(element.id+"s").innerHTML = "OFF"; 
    }
}

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