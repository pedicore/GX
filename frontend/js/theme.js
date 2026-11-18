const themeToggle = document.getElementById("themeBtn");
const htmlElement = document.documentElement;

const savedTheme = localStorage.getItem("theme");

if (savedTheme) {
  htmlElement.setAttribute("data-theme", savedTheme);
}

themeToggle.addEventListener("click", () => {
  const currentTheme = htmlElement.getAttribute("data-theme");
  if (currentTheme === "light") {
    
  } 

  //toggle theme
  const newTheme = currentTheme === "dark" ? "light" : "dark";
  htmlElement.setAttribute("data-theme", newTheme);
  localStorage.setItem("theme", newTheme);
});
