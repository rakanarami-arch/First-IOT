<?php
// Koneksi ke database
$conn = new mysqli("localhost", "root", "", "iot_monitoring");

// Mengambil 20 data terbaru yang diurutkan dari waktu paling akhir
$sql = "SELECT * FROM riwayat_sensor ORDER BY waktu_rekam DESC LIMIT 20";
$result = $conn->query($sql);
?>

<!DOCTYPE html>
<html lang="id">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Riwayat Sensor</title>
    <style>
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f8f9fa; padding: 40px; color: #333; }
        h2 { text-align: center; margin-bottom: 30px; }
        table { width: 80%; margin: 0 auto; border-collapse: collapse; background: white; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }
        th, td { padding: 15px; border: 1px solid #dee2e6; text-align: center; }
        th { background-color: #0d6efd; color: white; }
        tr:nth-child(even) { background-color: #f2f2f2; }
    </style>
</head>
<body>

    <h2>Rekaman Suhu & Kelembaban (20 Data Terakhir)</h2>

    <table>
        <tr>
            <th>Waktu Rekam</th>
            <th>Suhu (&deg;C)</th>
            <th>Kelembaban (%)</th>
        </tr>
        
        <?php 
        // Melakukan looping (perulangan) untuk mencetak setiap baris data
        if ($result->num_rows > 0) {
            while($row = $result->fetch_assoc()) {
                echo "<tr>";
                echo "<td>" . $row['waktu_rekam'] . "</td>";
                echo "<td>" . $row['suhu'] . "</td>";
                echo "<td>" . $row['kelembaban'] . "</td>";
                echo "</tr>";
            }
        } else {
            echo "<tr><td colspan='3'>Belum ada data dari sensor.</td></tr>";
        }
        $conn->close();
        ?>
    </table>

</body>
</html> 