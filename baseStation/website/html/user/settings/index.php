<?php
session_start();

// The user shouldn't be here if they are not logged in.
if(!isset($_SESSION['u_id'])) {
	header("Location: /user/login/");
	exit();
}

?>

<!DOCTYPE html>
<html lang="en">

<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
	<meta name="description" content="">
	<meta name="author" content="">
	<title>User Settings</title>
	<!-- Bootstrap core CSS-->
	<link href="/startbootstrap-sb-admin/vendor/bootstrap/css/bootstrap.min.css" rel="stylesheet">
	<!-- Custom fonts for this template-->
	<link href="/startbootstrap-sb-admin/vendor/font-awesome/css/font-awesome.min.css" rel="stylesheet" type="text/css">
	<!-- Custom styles for this template-->
    <link href="/startbootstrap-sb-admin/css/sb-admin.css" rel="stylesheet">
    
    <!-- Custom style for the website -->
    <link href="/css/style.min.css" rel="stylesheet">

	<!-- Favicon -->
	<link rel="apple-touch-icon" sizes="57x57" href="/favicon/apple-icon-57x57.png">
	<link rel="apple-touch-icon" sizes="60x60" href="/favicon/apple-icon-60x60.png">
	<link rel="apple-touch-icon" sizes="72x72" href="/favicon/apple-icon-72x72.png">
	<link rel="apple-touch-icon" sizes="76x76" href="/favicon/apple-icon-76x76.png">
	<link rel="apple-touch-icon" sizes="114x114" href="/favicon/apple-icon-114x114.png">
	<link rel="apple-touch-icon" sizes="120x120" href="/favicon/apple-icon-120x120.png">
	<link rel="apple-touch-icon" sizes="144x144" href="/favicon/apple-icon-144x144.png">
	<link rel="apple-touch-icon" sizes="152x152" href="/favicon/apple-icon-152x152.png">
	<link rel="apple-touch-icon" sizes="180x180" href="/favicon/apple-icon-180x180.png">
	<link rel="icon" type="image/png" sizes="192x192"  href="/favicon/android-icon-192x192.png">
	<link rel="icon" type="image/png" sizes="32x32" href="/favicon/favicon-32x32.png">
	<link rel="icon" type="image/png" sizes="96x96" href="/favicon/favicon-96x96.png">
	<link rel="icon" type="image/png" sizes="16x16" href="/favicon/favicon-16x16.png">
	<link rel="manifest" href="/manifest.json">
	<meta name="msapplication-TileColor" content="#ffffff">
	<meta name="msapplication-TileImage" content="/favicon/ms-icon-144x144.png">
	<meta name="theme-color" content="#ffffff">
</head>

<body class="bg-dark">
	<div class="container">
		<div class="card card-register mx-auto mt-5">
			<div class="card-header">
                <form action="/">
                    <button type="submit" class="close" aria-label="Close">
                        <span aria-hidden="true">&times;</span>
                    </button>
                </form>

                <h5 class="card-title">
                    Settings - <span id="user"></span>
                </h5>

                <ul class="nav nav-tabs card-header-tabs" id="myTab" role="tablist">
                    <li class="nav-item">
                        <a class="nav-link active" id="permission-tab" data-toggle="tab" href="#permission" role="tab" aria-controls="permission" aria-selected="true">permissions</a>
                    </li>
                    <li class="nav-item">
                        <a class="nav-link" id="password-tab" data-toggle="tab" href="#password" role="tab" aria-controls="password" aria-selected="false">Password</a>
                    </li>
                </ul>
            </div>
            <div class="card-body">
                <div class="tab-content" id="myTabContect">

                    <!-- Permissions -->
                    <div class="tab-pane fade show active" id="permission" role="tabpanel" aria-labelledby="permission-tab">
                        <div class="form-group form-check">
                            <label class="form-check-label">
                                <input class="form-check-input" type="checkbox" id="priv-add-usr" disabled>
                                Create Users
                            </label>
                        </div>
                        <div class="form-group form-check">
                            <label class="form-check-label">
                                <input class="form-check-input" type="checkbox" id="priv-edit-priv" disabled>
                                Change Privileges
                            </label>
                        </div>
                        <div class="form-group form-check">
                            <label class="form-check-label">
                                <input class="form-check-input" type="checkbox" id="priv-reboot" disabled>
                                Reboot System
                            </label>
                        </div>
                        <div class="form-group form-check">
                            <label class="form-check-label">
                                <input class="form-check-input" type="checkbox" id="priv-settings" disabled>
                                Change Settings
                            </label>
                        </div>
                            
                    </div>

                    <!-- Password -->
                    <div class="tab-pane fade" id="password" role="tabpanel" aria-labelledby="password-tab">
                        <div class="row">
                            <div class="col-12">
                                <div class="alert alert-danger hide" id="password-error"></div>
                            </div>
                            <div class="col-12">
                                <div class="alert alert-success hide" id="password-success">Successfully changed your password</div>
                            </div>
                        </div>

                        <form action="/php/login/changePass.php" method="POST">
                            <div class="form-group">
                                <label for="oldpass">Old Password</label>
                                <input class="form-control" name="oldpass" type="password" placeholder="Old Password">
                            </div>
                            <div class="form-group">
                                <div class="form-row">
                                    <div class="col-md-6">
                                        <label for="newpass">New Password</label>
                                        <input class="form-control" name="newpass" type="password" placeholder="New Password">
                                    </div>
                                    <div class="col-md-6">
                                        <label for="confpass">Confirm Password</label>
                                        <input class="form-control" name="confpass" type="password" placeholder="Confirm Password">
                                    </div>
                                </div>
                            </div>
                            <div class="form-group">
                                <button class="btn btn-danger btn-block" type="submit" name="submit">Change Password</button>
                            </div>

                        </form>
                    </div>
                </div>
            </div>
		</div>
	</div>
	<!-- Bootstrap core JavaScript-->
	<script src="/startbootstrap-sb-admin/vendor/jquery/jquery.min.js"></script>
	<script src="/startbootstrap-sb-admin/vendor/bootstrap/js/bootstrap.bundle.min.js"></script>
	<!-- Core plugin JavaScript-->
    <script src="/startbootstrap-sb-admin/vendor/jquery-easing/jquery.easing.min.js"></script>
    
	<!-- settings javascript -->
    <script src="settings.min.js"></script>
</body>

</html>