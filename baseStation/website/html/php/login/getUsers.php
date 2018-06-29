<?php

session_start();

if(isset($_SESSION['u_id']) && $_SESSION['u_priv_mod_priv'] == 'Y') {

    include_once "dbh.php";

    $query = "SELECT user_name, user_id, priv_create_user, priv_modify_priv, priv_reboot, priv_modify_settings FROM users";
    $result = $conn->query($query);

    class User {
        public $id;
        public $user;
        public $add_usr;
        public $mod_priv;
        public $reboot;
        public $mod_sett;
    }

    $users = array();

    while($row = $result->fetch_assoc()) {
        $usr = new User;
        $usr->id = $row['user_id'];
        $usr->user = $row['user_name'];
        $usr->add_usr = $row['priv_create_user'];
        $usr->mod_priv = $row['priv_modify_priv'];
        $usr->reboot = $row['priv_reboot'];
        $usr->mod_sett = $row['priv_modify_settings'];
        array_push($users, $usr);
    }

    echo json_encode($users);
    exit();
}