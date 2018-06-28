<?php

session_start();

$success = "success";

include_once 'dbh.php';

$username = $_POST['username'];
$pass = $_POST['password'];

// Error handlers

// Check for empty fields
if(empty($username)) {
    echo "No username given";
    exit();
}
if(empty($pass)) {
    echo "No password given";
    exit();
}

$stmt = $conn->prepare("SELECT * FROM users WHERE user_name=?");
$resultCheck = 0;
if($stmt != false) {
    $stmt->bind_param("s", $username);
    $result = $stmt->execute();

    $resultCheck = $result->num_rows;
} else {
    die($conn->error);
}

if($resultCheck == 0) {
    echo "User does not exist<br>";
    echo "$result->num_rows<br>";
    echo "'$username'";
    exit();
}


$row = $result->fetch_assoc();

$passCheck = password_verify($pass, $row['user_pass']);
if($passCheck == false) {
    echo "Password is incorrect";
    exit();
} else if($passCheck == true) {
    $_SESSION['u_id'] = $row['user_id'];
    $_SESSION['u_name'] = $row['user_name'];
    echo $success;
}