let set = false;

async function getCoordinates(){
    const result = await fetch("/coordinates");
    const h = await result.json();
    //console.log(h.el);
    //console.log(h.az)
    $('#elTxt').val(h.el);
    $('#azTxt').val(h.az);
}

setInterval(function () { if(!set){getCoordinates();} }, 2000);


async function sendDir(element_id){
   


    fetch("/dir", {method:"POST", body: JSON.stringify({"direction": element_id})});

    console.log(element_id);
    console.log(JSON.stringify({"direction": element_id}));
} 

async function sendGpredictEnabled(value){
   


    fetch("/gpredict_enable", {method:"POST", body: JSON.stringify({"gpredict": value})});

    //console.log(element_id);
    //console.log(JSON.stringify({"direction": element_id}));
} 

async function sendCoordinates(){
   

    let el = $('#elTxt').val();
    let az = $('#azTxt').val();
    fetch("/dirCustom", {method:"POST", body: JSON.stringify({"el": el, "az": az})});

    console.log(el);
    console.log(az);
    //console.log(JSON.stringify({"direction": element_id}));
} 



function show_manual(){

    $('.manual_control_buttons').css("visibility", "visible");
    $('.main-content-buttons').css("visibility", "hidden");
    $('#back_button').css("visibility", "visible");
    $('.left-col').css("visibility", "visible");


}

function back_to_menu(){

    $('.manual_control_buttons').css("visibility", "hidden");
    $('.main-content-buttons').css("visibility", "visible");
    $('#back_button').css("visibility", "hidden");
    $('.left-col').css("visibility", "hidden");
    $('#send_coordinates').css("visibility", "hidden");

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
        $('.left-col').css("visibility", "hidden");
        $('#send_coordinates').css("visibility", "hidden");
    }
    else {
        $('#gpredict_button').css("background", "#15D74C");
        $('.left-col').css("visibility", "visible");
        $('#send_coordinates').css("visibility", "visible");
        
    }

}

function toggle(){

    var color = $('#set_enable').css("background");

    if(rgb2hex(color).toUpperCase() == "#15D74C" ) {
        $('#set_enable').css("background", "#d7152c");
        $('#send_coordinates').css("visibility", "hidden");
        set = false;
    }
    else {
        $('#set_enable').css("background", "#15D74C");
        $('#send_coordinates').css("visibility", "visible");
        set = true;
    }
}

