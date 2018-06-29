<?php

$WEB = "/user/register/";
$REG = "?register";

if($_SERVER['REQUEST_METHOD'] === 'POST') {

    if(isset($_POST['submit'])) {

        include_once "dbh.php";

        $username = $conn->real_escape_string($_POST['username']);
        $pass = $conn->real_escape_string($_POST['password']);
        $pass2 = $conn->real_escape_string($_POST['passwordc']);

        // Error handlers

        // Check for empty fields
        if(empty($username) || empty($pass) || empty($pass2)) {
            header("Location: $WEB$REG=empty");
            $conn->close();
            exit();
        }

        // Check for valid characters
        if(!preg_match("/^[a-zA-Z0-9]*$/", $username)) {
            header("Location: $WEB$REG=invalid");
            $conn->close();
            exit();
        }

        // check for matching passwords

        if($pass != $pass2) {
            header("Location: $WEB$REG=dontmatch");
            $conn->close();
            exit();
        }

        $stmt = "SELECT * FROM users WHERE user_name='$username'";
        $result = $conn->query($stmt);

        if($result->num_rows > 0) {
            header("Location: $WEB$REG=usertaken");
            $conn->close();
            exit();
        }

        // Hash the password

        $hashpwd = password_hash($pass, PASSWORD_DEFAULT);
        
        // Add user

        $stmt = "INSERT INTO users (user_name, user_pass) VALUES ('$username', '$hashpwd')";
        $conn->query($stmt);

        if($conn->affected_rows > 0) {
            header("Location: $WEB$REG=success");
        } else {
            header("Location: $WEB$REG=fail");
        }
        $conn->close();
        exit();
        


    }

}

header("Location: $WEB");
exit();