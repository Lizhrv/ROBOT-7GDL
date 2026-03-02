<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <title>Control de Robot - Arduino R4</title>
    <style>
        /* Tu CSS actual se mantiene igual */
        body { font-family: sans-serif; background: #1a1a1a; color: white; text-align: center; }
        .dashboard { max-width: 600px; margin: 20px auto; padding: 20px; border-radius: 12px; background: #2d2d2d; }
        button { padding: 15px 30px; margin: 10px; font-size: 18px; cursor: pointer; border-radius: 8px; border: none; }
        .btn-start { background: #2ecc71; color: white; }
        .btn-rutina2 { background: #3498db; color: white; }
        .btn-reset { background: #e74c3c; color: white; }
        input { padding: 10px; border-radius: 5px; border: 1px solid #444; background: #333; color: #00ff00; text-align: center; }
    </style>
</head>
<body>

    <div class="dashboard">
        <h2>Panel de Control Robot</h2>
        
        <div class="config-bar">
            <span>IP DEL ROBOT:</span>
            <input type="text" id="ipInput" value="192.168.100.50">
        </div>

        <hr>

        <button class="btn-start" onclick="enviarComando('start')">EJECUTAR RUTINA 1</button>
        <button class="btn-rutina2" onclick="enviarComando('rutina2')">EJECUTAR RUTINA 2</button>
        <button class="btn-reset" onclick="enviarComando('reset')">PARADA DE EMERGENCIA</button>

        <div id="status" style="margin-top: 20px; color: #888;">Listo para conectar...</div>
    </div>

    <script>
        function enviarComando(ruta) {
            const ip = document.getElementById('ipInput').value;
            const statusDiv = document.getElementById('status');
            const url = `http://${ip}/${ruta}`;

            statusDiv.innerText = `Enviando a ${url}...`;
            statusDiv.style.color = "#f1c40f";

            // Usamos mode: 'no-cors' para evitar bloqueos del navegador
            fetch(url, { mode: 'no-cors' })
                .then(() => {
                    statusDiv.innerText = "Comando enviado con éxito";
                    statusDiv.style.color = "#2ecc71";
                })
                .catch(err => {
                    statusDiv.innerText = "Error: Verifica la conexión";
                    statusDiv.style.color = "#e74c3c";
                    console.error("Error de red:", err);
                });
        }
    </script>
</body>
</html>