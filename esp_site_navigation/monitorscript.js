const LOCAL_STORAGE_KEY = 'deviceList';
const cardGrid = document.getElementById('card-grid');

try {
  socket = new WebSocket('ws://' + window.location.hostname + ':81/');
  socket.onmessage = (event) => {
    console.log({ event });
    // receive the data from the server
    const data = JSON.parse(event.data);
    console.log({ data });
    saveToLocalStorage({ ...data, updated: new Date() });
    const dataFromStorage = getFromLocalStorage();
    console.log({ dataFromStorage });
    cardGrid.innerHTML = '';
    dataFromStorage.map((deviceData) => {
      const card = createCard(deviceData);
      cardGrid.appendChild(card);
    });
    //Updates the last time data was recieved
    // lastResponseElement.textContent = dateformat(new Date(), 'DD.MM.YYYY');
  };
  console.log('Success start!');
} catch (e) {
  console.error(e);
}

function saveToLocalStorage(deviceData) {
  const previousDeviceData = localStorage.getItem(LOCAL_STORAGE_KEY);
  const previousDeviceDataArray = JSON.parse(previousDeviceData) ?? [];

  const indexOfDeviceWithThisId = previousDeviceDataArray
    .map(({ id }) => id)
    .indexOf(deviceData.id);

  const newDeviceDataArray = [];
  if (indexOfDeviceWithThisId !== -1) {
    previousDeviceDataArray.splice(indexOfDeviceWithThisId, 1);
    previousDeviceDataArray.push(deviceData);
    newDeviceDataArray.push(...previousDeviceDataArray);
  } else {
    newDeviceDataArray.push(...previousDeviceDataArray);
    newDeviceDataArray.push(deviceData);
  }
  console.log(
    newDeviceDataArray,
    newDeviceDataArray.sort(({ id: id1 }, { id: id2 }) => id1 - id2)
  );

  localStorage.setItem(LOCAL_STORAGE_KEY, JSON.stringify(newDeviceDataArray.sort(({ id }) => id)));
}

function getFromLocalStorage() {
  return JSON.parse(localStorage.getItem(LOCAL_STORAGE_KEY) ?? []);
}

function createCard({ id, temperature, humidity, ambientLight, updated }) {
  const card = createElement({ className: 'card' });
  const dataList = createElement({ className: 'data-list' });

  dataList.appendChild(createDataItem('Reciver ID:', id));
  dataList.appendChild(createDataItem('Ambient temperature:', `${temperature}°C`));
  dataList.appendChild(createDataItem('Ambient humidity:', `${humidity} %`));
  dataList.appendChild(createDataItem('Ambient light:', `${ambientLight}  %`));
  dataList.appendChild(createDataItem('Дата последнего изменения:', updated));
  card.appendChild(dataList);

  return card;
}

function createDataItem(name, value) {
  const item = createElement({ className: 'data-item' });
  const itemName = createElement({ className: 'item-name', textContent: name });
  const itemValue = createElement({ className: 'item-value', textContent: value });
  item.appendChild(itemName);
  item.appendChild(itemValue);

  return item;
}

function createElement({ tagName = 'div', className, textContent }) {
  const element = document.createElement(tagName);
  element.className = className;
  element.textContent = textContent;

  return element;
}
