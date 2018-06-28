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

function submitLogin() {

    var origin = findGetParameter("origin");
    if (origin == null) {
        origin = "/"
    }

    $.post("/php/login/login.php", 
    {
        username: $("input[name=username]").val(),
        password: $("input[name=pwd]").val()
    }, function(data, status) {
        if(data == "success") {
            $("#login-error").hide();
            window.location.replace(origin);
        } else {
            $("#login-error").html(data);
            $("#login-error").show();
        }
        
    });
}