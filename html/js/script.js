async function getT(){
    const result = await fetch("/hello");
    const h = await result.json();
    console.log(h);
}

getT();

let direction = 0;
async function sendDir(element_id){
   


    fetch("/dir", {method:"POST", body: JSON.stringify({"direction": element_id})});

    console.log(element_id);
    console.log(JSON.stringify({"direction": element_id}));
} 



function show_manual(){

    $('.manual_control_buttons').css("visibility", "visible");
    $('.main-content-buttons').css("visibility", "hidden");
    $('#back_button').css("visibility", "visible");


}

function back_to_menu(){

    $('.manual_control_buttons').css("visibility", "hidden");
    $('.main-content-buttons').css("visibility", "visible");
    $('#back_button').css("visibility", "hidden");

}

function rgb2hex(rgb) {
    rgb = rgb.match(/^rgb\((\d+),\s*(\d+),\s*(\d+)\)/);
    function hex(x) {
        return ("0" + parseInt(x).toString(16)).slice(-2);
    }
    return "#" + hex(rgb[1]) + hex(rgb[2]) + hex(rgb[3]);
}

function gpredict_toggle(){

    var color = $('#gpredict_button').css("background");

    if(rgb2hex(color).toUpperCase() == "#15D74C" ) {
        $('#gpredict_button').css("background", "#d7152c");
    }
    else {
        $('#gpredict_button').css("background", "#15D74C");
    }

}

