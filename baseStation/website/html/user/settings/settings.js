function findGetParameter(parameterName) {
    var result = null,
        tmp = [];
    location.search
        .substr(1)
        .split("&")
        .forEach(function (item) {
          tmp = item.split("=");
          if (tmp[0] === parameterName) result = decodeURIComponent(tmp[1]);
        });
    return result;
}

function getEnumVal(value) {
    return value == "Y";
}

function setPriveleges() {
    $.get("/php/login/getPermissions.php", function(data) {
        var priv = JSON.parse(data);

        $("#priv-add-usr").prop('checked', getEnumVal(priv.add_usr));
        $("#priv-edit-priv").prop('checked', getEnumVal(priv.mod_priv));
        $("#priv-reboot").prop('checked', getEnumVal(priv.reboot));
        $("#priv-settings").prop('checked', getEnumVal(priv.mod_sett));
    });
}

function setUser() {
    $.get("/php/login/isLoggedIn.php", function(data) {
        if(data != "false") {
            $("#user").html(data);
        }
    });
}

function setPasswordAlert() {
    var status = findGetParameter("passstat");

    if(status != null && status != "") {
        $(".nav-tabs a[href='#password']").tab("show");

        if(status == "success") {
            $("#password-error").hide();
            $("#password-success").show();
        } else {

            var message = "";

            if(status == "incorrect") {
                message = "Incorrect Password";
            }

            else if(status == "match") {
                message = "Passwords do not match";
            }

            else if(status == "fail") {
                message = "Something went wrong when setting your password";
            }

            $("#password-success").hide();
            $("#password-error").html(message);
            $("#password-error").show();
        }
    }
}

window.addEventListener("load", function(event) {
    setUser();
    setPriveleges();
    setPasswordAlert();
});