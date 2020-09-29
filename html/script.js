async function getT(){
    const result = await fetch("/hello");
    const h = await result.json();
    console.log(h);
}

getT();

let direction = 0;
async function sendDir(){
    direction = document.getElementById("dir_text").value;
    
    
    fetch("/dir", {method:"POST", body: JSON.stringify({"direction": direction})});
    
    console.log(direction);
    console.log(JSON.stringify({"direction": direction}));
}