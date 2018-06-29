<?php

session_start();

if(isset($_SESSION['u_id']) && $_SESSION['u_priv_mod_priv'] == "Y") {

    include_once "dbh.php";

    $uid = $_POST['user_id'];
    $priv_add_usr = $conn->real_escape_string($_POST['priv_add_usr']);
    $priv_mod_priv = $conn->real_escape_string($_POST['priv_mod_priv']);
    $priv_reboot = $conn->real_escape_string($_POST['priv_reboot']);
    $priv_mod_sett = $conn->real_escape_string($_POST['priv_mod_sett']);

    if(empty($uid) || empty($priv_add_usr) || empty($priv_mod_priv) || empty($priv_reboot) || empty($priv_mod_sett)) {
        echo "Not enough parameters";
        exit();
    }

    if(!preg_match("/^(?:Y\b|N\b)/", $priv_add_usr)
            || !preg_match("/^(?:Y\b|N\b)/", $priv_mod_priv)
            || !preg_match("/^(?:Y\b|N\b)/", $priv_reboot)
            || !preg_match("/^(?:Y\b|N\b)/", $priv_mod_sett)) {
        echo "Parameters must be Y or N";
        exit();
    }

    $query = "UPDATE users SET priv_create_user='$priv_add_usr',priv_modify_priv='$priv_mod_priv',priv_reboot='$priv_reboot',priv_modify_settings='$priv_mod_sett' WHERE user_id=$uid";

    $conn->query($query);

    if($conn->affected_rows == 1) {
        echo "success";
    } else if($conn->affected_rows > 0) {
        echo "Changed more than one users permission";
    } else if($conn->affected_rows == 0) {
        echo "Did not change permissions";
    } else {
        echo $conn->error;
    }

    exit();


}

echo "now now";
exit();