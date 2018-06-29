<?php

session_start();

include_once "dbh.php";

$web="/user/settings/";
$stat="?passstat";

if(isset($_SESSION['u_id']) && isset($_POST['submit'])) {
    $uid = $_SESSION['u_id'];
    $oldpass = $conn->real_escape_string($_POST['oldpass']);
    $newpass = $conn->real_escape_string($_POST['newpass']);
    $conpass = $conn->real_escape_string($_POST['confpass']);

    if(empty($oldpass)) {
        header("Location: $web$stat=incorrect");
        $conn->close();
        exit();
    }

    if(empty($newpass) || empty($conpass) || $newpass != $conpass) {
        header("Location: $web$stat=match");
        $conn->close();
        exit();
    }

    $result = $conn->query("SELECT * FROM users WHERE user_id='$uid'");

    if($result->num_rows == 0) {
        header("Location: /");
        $conn->close();
        exit();
    }

    $row = $result->fetch_assoc();

    $passCheck = password_verify($oldpass, $row['user_pass']);
    if($passCheck == false) {
        header("Location: $web$stat=incorrect");
        $conn->close();
        exit();
    } else if($passCheck == true) {
        $newHashPass = password_hash($newpass, PASSWORD_DEFAULT);

        $query = "UPDATE users SET user_pass = '$newHashPass' WHERE users.user_id = $uid";
        $conn->query($query);

        if($conn->affected_rows > 0) {
            header("Location: $web$stat=success");
        } else {
            header("Location: $web$stat=fail");
        }

        $conn->close();
        exit();
    }
}

header("Location: /");
exit();