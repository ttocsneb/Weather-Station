<?php
session_start();

if(!isset($_SESSION['u_id'])) {
	header("Location: /");
	exit();
}

?>

<!DOCTYPE html>
<html lang="en">

<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="wnameth=device-wnameth, initial-scale=1, shrink-to-fit=no">
	<meta name="robots" content="noindex" />
	<title>Register User</title>
	<!-- Bootstrap core CSS-->
	<link href="/startbootstrap-sb-admin/vendor/bootstrap/css/bootstrap.min.css" rel="stylesheet">
	<!-- Custom fonts for this template-->
	<link href="/startbootstrap-sb-admin/vendor/font-awesome/css/font-awesome.min.css" rel="stylesheet" type="text/css">
	<!-- Custom styles for this template-->
	<link href="/startbootstrap-sb-admin/css/sb-admin.css" rel="stylesheet">

	<!-- Custom styles for the website -->
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
            Register an Account
			</div>
			<div class="card-body">
				<div class="alert alert-danger hide" id="register-fail"></div>
				<div class="alert alert-success hide" id="register-success">Successfully created Account</div>
				<form action="/php/login/createUser.php" method="POST">
					<div class="form-group">
						<label for="username">User Name</label>
						<input class="form-control" name="username" type="text" placeholder="Username">
					</div>
					<div class="form-group">
						<div class="form-row">
							<div class="col-md-6">
								<label for="password">Password</label>
								<input class="form-control" name="password" type="password" placeholder="Password">
							</div>
							<div class="col-md-6">
								<label for="passwordc">Confirm password</label>
								<input class="form-control" name="passwordc" type="password" placeholder="Confirm password">
							</div>
						</div>
					</div>
					<input class="btn btn-primary btn-block" type="submit" name="submit" value="Register">
				</form>
				<div class="text-center">
					<a class="d-block small mt-3" href="/user/login/">Login Page</a>
				</div>
			</div>
		</div>
	</div>
	<!-- Bootstrap core JavaScript-->
	<script src="/startbootstrap-sb-admin/vendor/jquery/jquery.min.js"></script>
	<script src="/startbootstrap-sb-admin/vendor/bootstrap/js/bootstrap.bundle.min.js"></script>
	<!-- Core plugin JavaScript-->
	<script src="/startbootstrap-sb-admin/vendor/jquery-easing/jquery.easing.min.js"></script>

	<!-- Page js -->
	<script src="register.min.js"></script>
</body>

</html>
