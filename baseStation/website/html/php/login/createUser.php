<?php

$WEB = "../../register.html";
$REG = "?register";

if($_SERVER['REQUEST_METHOD'] === 'POST') {

    if(isset($_POST['submit'])) {

        include_once "dbh.php";

        $username = $_POST['username'];
        $pass = $_POST['password'];
        $pass2 = $_POST['passwordc'];

        // Error handlers

        // Check for empty fields
        if(empty($username) || empty($pass) || empty($pass2)) {
            header("Location: $WEB$REG=empty");
            exit();
        }

        // Check for valid characters
        if(!preg_match("/^[a-zA-Z0-9]*$/", $username)) {
            header("Location: $WEB$REG=invalid");
            exit();
        }

        // check for matching passwords

        if($pass != $pass2) {
            header("Location: $WEB$REG=donntmatch");
            exit();
        }

        $stmt = $conn->prepare("SELECT * FROM users WHERE user_name=?");
        if($stmt != false) {
            $stmt->bind_param("s", $username);
            $result = $stmt->execute();
            $resultCheck = mysqli_num_rows($result);

            if($resultCheck > 0) {
                header("Location: $WEB$REG=usertaken");
                exit();
            }

            $stmt->close();
        } else {
            die($conn->error);

        }
        

        // Hash the password

        $hashpwd = password_hash($pass, PASSWORD_DEFAULT);
        
        // Add user

        $stmt = $conn->prepare("INSERT INTO users (user_name, user_pass) VALUES (?, ?)");
        if($stmt != false) {
            $stmt->bind_param("ss", $username, $hashpwd);
            $stmt->execute();
            $stmt->close();
        } else {
            // header("Location: $WEB$REG=error");
            // exit();
            die($conn->error);
        }

        header("Location: $WEB$REG=success");
        exit();
        


    }

}

header("Location: $WEB");
exit();