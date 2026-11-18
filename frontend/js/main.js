const toggleButton = document.getElementById("sidebar-toggle");
const appLayout = document.querySelector(".app-layout");

toggleButton.addEventListener("click", (e) => {
  e.preventDefault();
  appLayout.classList.toggle("collapsed");
  appLayout.classList.toggle("smooth");
});
//============
// CPU Chart
//============

const ctx = document.getElementById("cpu-chart");

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

  const values = cpuChart.data.datasets[0].data;
  const labels = cpuChart.data.labels;

  values.push(newCpuValue);
  
  labels.push(new Date().toLocaleTimeString());

  if (values.length > 5) {
    values.shift();
    labels.shift();
  }

  cpuChart.update("none");
}, 1000);