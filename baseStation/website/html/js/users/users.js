
function setLoginButton() {
    $.get("/php/login/isLoggedIn.php", function(data) {
        if(data != "false") {
            console.log("Logged in");
            $(".user-loggedin").show();
            $("#user-login").hide();
        } else {
            console.log("Logged out");
            $(".user-loggedin").hide();
            $("#user-login").show();
        }
    });
}

function logout() {
    $.get("/php/login/logout.php", function(data) {
        location.reload(true);
    });
}

window.addEventListener("load", function(event) {
    setLoginButton();
});