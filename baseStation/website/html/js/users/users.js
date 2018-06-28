var login = document.getElementById("user-login");
var logout = document.getElementById("user-logout");

window.addEventListener("load", function(event) {
    $.get("/php/login/isLoggedIn.php", function(data) {
        if(data == "true") {
            login.style.display='none';
            logout.style.display='block';
        } else {
            login.style.display='block';
            logout.style.display='none';
        }
    });
});