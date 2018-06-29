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

window.addEventListener("load", function(event) {
    var status = findGetParameter("register");

    if(status == null) {
        return;
    }

    if(status == "success") {
        $("#register-success").show();
        $("#register-fail").hide();
    } else if(status != ""){
        var message = "Something went wrong";

        if(status == "empty") {
            message = "Empty fields";
        } else if(status == "invalid") {
            message = "Can't use special characters in username";
        } else if(status == "dontmatch") {
            message = "Passwords don't match";
        } else if(status == "usertaken") {
            message = "User already exists";
        }

        $("#register-success").hide();
        $("#register-fail").html(message);
        $("#register-fail").show();
    }
});