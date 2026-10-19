<?php
// Konfigurasi database bawaan Laragon
$host = "localhost";
$user = "root";
$pass = "";
$db   = "iot_monitoring";

// Membuat koneksi ke database
$conn = new mysqli($host, $user, $pass, $db);

// Memeriksa koneksi
if ($conn->connect_error) {
    die("Koneksi gagal: " . $conn->connect_error);
}

// Memeriksa apakah ada data 'suhu' dan 'kelembaban' yang dikirim melalui metode POST
if (isset($_POST['suhu']) && isset($_POST['kelembaban'])) {
    
    // Menangkap data
    $suhu = $_POST['suhu'];
    $kelembaban = $_POST['kelembaban'];

    // Menyiapkan query untuk memasukkan data ke tabel
    $sql = "INSERT INTO riwayat_sensor (suhu, kelembaban) VALUES ('$suhu', '$kelembaban')";

    // Menjalankan query dan memberikan respons
    if ($conn->query($sql) === TRUE) {
        echo "Sukses: Data tersimpan di database!";
    } else {
        echo "Error: " . $sql . "<br>" . $conn->error;
    }

} else {
    echo "Gagal: Parameter suhu atau kelembaban tidak ditemukan.";
}

$conn->close();
?>