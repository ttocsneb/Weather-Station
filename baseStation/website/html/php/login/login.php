<?php

session_start();

$success = "success";

include_once 'dbh.php';

$username = $conn->real_escape_string($_POST['username']);
$pass = $conn->real_escape_string($_POST['password']);

// Error handlers

// Check for empty fields
if(empty($username)) {
    echo "No username given";
    $conn->close();
    exit();
}
if(empty($pass)) {
    echo "No password given";
    $conn->close();
    exit();
}

// $stmt = $conn->prepare("SELECT * FROM users WHERE user_name = ?");
// $stmt->bind_param("s", $usr);
// $usr = $username;
// $result = $stmt->execute();


$result = $conn->query("SELECT * FROM users WHERE user_name='$username'");

if($result->num_rows == 0) {
    echo "User does not exist";
    $conn->close();
    exit();
}


$row = $result->fetch_assoc();

$passCheck = password_verify($pass, $row['user_pass']);
if($passCheck == false) {
    echo "Password is incorrect";
    $conn->close();
    exit();
} else if($passCheck == true) {
    $_SESSION['u_id'] = $row['user_id'];
    $_SESSION['u_name'] = $row['user_name'];
    $_SESSION['u_priv_add_usr'] = $row['priv_create_user'];
    $_SESSION['u_priv_mod_priv'] = $row['priv_modify_priv'];
    $_SESSION['u_priv_reboot'] = $row['priv_reboot'];
    $_SESSION['u_priv_mod_sett'] = $row['priv_modify_settings'];
    echo $success;
    $conn->close();
}