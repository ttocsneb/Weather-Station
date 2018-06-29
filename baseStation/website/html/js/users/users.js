
function setLoginButton() {
    $.get("/php/login/isLoggedIn.php", function(data) {
        if(data != "false") {
            console.log("Logged in");
            $(".user-loggedin").show();
            $("#user-login").hide();

            $.get("/php/login/getPermissions.php", function(data) {
                var priv = JSON.parse(data);
                if(priv.mod_priv == "Y")
                    $("#user-change-priv").show();
                if(priv.add_usr == "Y")
                    $("#user-add-user").show();
            });

        } else {
            console.log("Logged out");
            $(".user-loggedin").hide();
            $(".user-loggedin-priv").hide();
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