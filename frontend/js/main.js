const toggleButton = document.getElementById("sidebar-toggle");
const appLayout = document.querySelector(".app-layout");
document.documentElement.classList.remove("loading");

toggleButton.addEventListener("click", (e) => {
  e.preventDefault();
  appLayout.classList.toggle("collapsed");
  appLayout.classList.toggle("smooth");
});
//============
// Files activity
//============
const activityBtn = document.getElementById("activityBtn");
const arrow = document.getElementById("arrow");
const recentFile = document.getElementById("file");

activityBtn.addEventListener("click", () => {
  arrow.classList.toggle("rotated");
  recentFile.classList.toggle("collapsed-file");
});

//============
// CPU Chart
//============

const ctx = document.getElementById("cpu-chart");
const rchartt = document.getElementById("ram-chart");

const cpuChart = new Chart(ctx, {
  type: "line",

  data: {
    labels: ["0s", "1s", "2s", "3s", "4s"],
    datasets: [
      {
        label: "CPU Usage",
        data: [100, 30, 25, 60, 40],
        borderColor: "#38bdf8",
        backgroundColor: "rgba(56, 189, 248, 0.15)",
        tension: 0,
        fill: true,
        pointRadius: 3,
      },
    ],
  },

  options: {
    responsive: true,
    maintainAspectRatio: false,
    animation: false,
    scales: {
      y: {
        min: 0,
        max: 100,
      },
    },
  },
});

//============
// RAM Chart
//============

const ramchart = new Chart(rchartt, {
  type: "line",

  data: {
    labels: ["0s", "1s", "2s", "3s", "4s"],
    datasets: [
      {
        label: "RAM Usage",
        data: [100, 30, 25, 60, 40],
        borderColor: "#38bdf8",
        backgroundColor: "rgba(56, 189, 248, 0.15)",
        tension: 0,
        fill: true,
        pointRadius: 3,
      },
    ],
  },

  options: {
    responsive: true,
    maintainAspectRatio: false,
    animation: false,
    scales: {
      y: {
        min: 0,
        max: 100,
      },
    },
  },
});

//============
// Live CPU Data
//============

setInterval(() => {
  const newCpuValue = Math.floor(Math.random() * 101);
  const newRamValue = Math.floor(Math.random() * 101);

  const values = cpuChart.data.datasets[0].data;
  const labels = cpuChart.data.labels;

  const valuesRam = ramchart.data.datasets[0].data;
  const labelsRam = ramchart.data.labels;

  valuesRam.push(newRamValue);
  labelsRam.push(new Date().toLocaleTimeString());

  values.push(newCpuValue);
  labels.push(new Date().toLocaleTimeString());

  if (values.length > 5) {
    values.shift();
    labels.shift();
  }
  if (valuesRam.length > 5) {
    valuesRam.shift();
    labelsRam.shift();
  }

  cpuChart.update("none");
  ramchart.update("none");
}, 1000);
