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

    //Progress bar
    const progressWrapper = document.createElement("div");
    const svgParent = document.createElementNS(
      "http://www.w3.org/2000/svg",
      "svg",
    );
    const progressTrack = document.createElementNS(
      "http://www.w3.org/2000/svg",
      "circle",
    );
    const progressBar = document.createElementNS(
      "http://www.w3.org/2000/svg",
      "circle",
    );
    
    const percentage = 65
    const offset = (1 - percentage / 100 )   * 125.66
    
    svgParent.setAttribute("viewBox", "0 0 48 48")
    progressTrack.setAttribute("cx", 24);
    progressTrack.setAttribute("cy" , 24)
    progressTrack.setAttribute("r" , 20)
    progressTrack.setAttribute("fill" , "none")
    progressTrack.setAttribute("stroke-width" , 4)
    progressTrack.setAttribute("stroke" , "#374151")



    progressBar.setAttribute("cx", 24);
    progressBar.setAttribute("cy" , 24)
    progressBar.setAttribute("r" , 20)
    progressBar.setAttribute("fill" , "none")
    progressBar.setAttribute("stroke-width" , 4)
    progressBar.setAttribute("stroke" , "#38bdf8")
    progressBar.setAttribute("stroke-dasharray" , 125.66)
    progressBar.setAttribute("stroke-dashoffset" , offset)
 



    cardHeader.appendChild(progressWrapper);
    svgParent.appendChild(progressTrack);
    svgParent.appendChild(progressBar);
    progressWrapper.appendChild(iconElement);
    progressWrapper.appendChild(svgParent);

    progressWrapper.classList.add("progress-wrapper");
    progressTrack.classList.add("progress-track");
    progressBar.classList.add("progress-bar");

    // Card title
    const cardTitle = document.createElement("h3");
    cardTitle.classList.add("card-title");
    cardTitle.textContent = this.title;

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
  const card = new SystemCard(item.title, item.value, item.icon);

  systemCards.appendChild(card.render());
});
