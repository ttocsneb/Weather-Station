var privileges = null;
var user = null;

function selectUser() {
    var selectedUser = $("input[type=radio][name=selectUsers]:checked").val();
    error = $("#form-error");
    if(selectedUser == null) {
        error.html("You need to select a user");
        error.show();
        $("#form-success").hide();
        return;
    }
    error.hide();
    console.log("selecting " + selectedUser);

    for(var i in privileges) {
        var usr = privileges[i];
        if(usr.user == selectedUser) {
            user = usr;
            loadSettings(usr);
            break;
        }
    }
}

function toEnum(val) {
    if(val) {
        return "Y";
    } else {
        return "N";
    }
}

function updateSettings() {
    $.post("/php/login/changePrivileges.php",{
        "user_id": user.id,
        "priv_add_usr": toEnum($("#priv-add-usr").prop('checked')),
        "priv_mod_priv": toEnum($("#priv-mod-priv").prop('checked')),
        "priv_reboot": toEnum($("#priv-reboot").prop('checked')),
        "priv_mod_sett": toEnum($("#priv-mod-sett").prop('checked'))
    }, function(data, status) {
        console.log(data);
        console.log(status);
        var success = $("#form-success");
        var fail = $("#form-error");
        if(data == "success") {
            success.html(`Successfully changed ${user.user}'s privileges`)
            success.show();
            loadUsers();
            return;
        }
        success.hide();
        fail.html(data);
        fail.show();
        loadUsers();
    });
}

function cancelPrivileges() {
    $("#form-error").html("Cancelled changes");
    $("#form-error").show();
    $("#form-success").hide();
    loadUsers();
}

function getChecked(priv) {
    if(priv == "Y") {
        return "checked";
    }
    return "";
}

function loadSettings(user) {
    var html = `
    <form action="javascript:;" onsubmit="updateSettings()">
        <div class="form-group">
            <input type="text" readonly class="form-control" id="staticUser" value="${user.user}">
        </div>
        <div class="form-group form-check">
            <input class="form-check-input" type="checkbox" id="priv-add-usr" ${getChecked(user.add_usr)}>
            <label class="form-check-label" for="priv-add-usr">
                Add Users
            </label>
        </div>
        <div class="form-group form-check">
            <input class="form-check-input" type="checkbox" id="priv-mod-priv" ${getChecked(user.mod_priv)}>
            <label class="form-check-label" for="priv-mod-priv">
                Modify Privileges
            </label>
        </div>
        <div class="form-group form-check">
            <input class="form-check-input" type="checkbox" id="priv-reboot" ${getChecked(user.reboot)}>
            <label class="form-check-label" for="priv-reboot">
                Reboot System
            </label>
        </div>
        <div class="form-group form-check">
            <input class="form-check-input" type="checkbox" id="priv-mod-sett" ${getChecked(user.mod_sett)}>
            <label class="form-check-label" for="priv-mod-sett">
                Change Settings
            </label>
        </div>
        <button class="btn btn-warning" type="submit">Update Privileges</button>
        <button class="btn btn-secondary" type="button" onclick="cancelPrivileges()">Back</button>
    </form>
    `;
    $("#privilege-body").html(html);
}

function createUserRadio(user) {
    var html = `
    <div class="form-group form-check">
        <input class="form-check-input" type="radio" name="selectUsers" id="user${user}" value="${user}">
        <label class="form-check-label" for="user${user}">
            ${user}
        </label>
    </div>
    `;
    return html;
}

function loadUsers() {
    $.get("/php/login/getUsers.php", function(data) {
        if(data != "") {
            var obj = JSON.parse(data);
            privileges = obj;

            var html = `
                <form action="javascript:;" onsubmit="selectUser()">
            `;

            for(var i in obj) {
                var user = obj[i];
                html += createUserRadio(user.user);
            }

            html += `
                <button class="btn btn-primary" type="submit">Edit User Privileges</button>
            </form>
            `;

            $("#privilege-body").html(html);
            
            $.get("/php/login/isLoggedIn.php", function(data) {
                if(data != "false") {
                    var you = $(`#user${data}`);
                    you.prop('disabled', true);
                    you.parent().find("label").html(`${data} (you)`)
                }
            });
        }
    });
}

window.addEventListener("load", function(event) {
    loadUsers();
});