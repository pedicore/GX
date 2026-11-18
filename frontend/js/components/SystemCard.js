
const systemCards = document.getElementById("system-cards");

class SystemCard {
  constructor(title, value, icon) {
    this.title = title;
    this.value = value;
    this.icon = icon;
    this.cardValue = null;
  }

  render() {
    // Main card
    const divElement = document.createElement("div");
    divElement.classList.add("system-card");

    // Card header
    const cardHeader = document.createElement("div");
    cardHeader.classList.add("card-header");

    // Card icon
    const iconElement = document.createElement("span");
    iconElement.classList.add("card-icon");
    iconElement.style.maskImage = `url("${this.icon}")`;

    // Card title
    const cardTitle = document.createElement("h3");
    cardTitle.classList.add("card-title");
    cardTitle.textContent = this.title;

    cardHeader.appendChild(iconElement);
    cardHeader.appendChild(cardTitle);

    // Card value
    this.cardValue = document.createElement("p");
    this.cardValue.classList.add("card-value");
    this.cardValue.textContent = this.value;

    // Assemble card
    divElement.appendChild(cardHeader);
    divElement.appendChild(this.cardValue);

    return divElement;
  }

  updateValue(newValue) {
    this.value = newValue;

    if (this.cardValue) {
      this.cardValue.textContent = newValue;
    }
  }
}

// Card data
const cardData = [
  {
    title: "Ram Usage",
    value: "0%",
    icon: "assets/icons/memory-stick.svg",
  },
  {
    title: "CPU Usage",
    value: "0%",
    icon: "assets/icons/cpu.svg",
  },
  {
    title: "Disk Usage",
    value: "0%",
    icon: "assets/icons/hard-drive.svg",
  },
  {
    title: "Graphic Card",
    value: "0%",
    icon: "assets/icons/gpu.svg",
  },
];

// Render cards
cardData.forEach((item) => {
  const card = new SystemCard(
    item.title,
    item.value,
    item.icon
  );

  systemCards.appendChild(card.render());
});
