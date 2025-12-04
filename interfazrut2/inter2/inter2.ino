<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Yarbito Control Remoto</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <link href="https://fonts.googleapis.com/css2?family=Orbitron:wght@400;700&family=Roboto:wght@300;400;700&display=swap" rel="stylesheet">
    <style>
        /* Variables CSS para facilitar cambios de color */
        :root {
            --primary-bg: linear-gradient(135deg, #0f0c29 0%, #302b63 100%);
            --secondary-bg: #1a1a2e;
            --accent-color-1: #00f0ff; /* Azul neón */
            --accent-color-2: #ff007f; /* Rosa neón */
            --button-green: #39ff14; /* Verde neón */
            --button-blue: #00bfff; /* Azul claro neón */
            --button-red: #ff073a; /* Rojo neón */
            --text-light: #e0e0e0;
            --text-dark: #121212;
            --shadow-light: rgba(0, 240, 255, 0.4);
        }

        /* ESTILOS GENERALES Y LAYOUT BASE (MÓVIL) */
        body {
            font-family: 'Roboto', sans-serif;
            margin: 0;
            padding: 0;
            background: var(--primary-bg);
            color: var(--text-light);
            display: flex;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            padding: 20px;
        }

        /* CONTENEDOR PRINCIPAL */
        .container {
            background-color: var(--secondary-bg);
            padding: 30px;
            border-radius: 15px;
            box-shadow: 0 0 40px var(--shadow-light); 
            width: 100%;
            max-width: 900px;
            border: 2px solid var(--accent-color-1); 
            text-align: center;
            
            /* CSS Grid para organizar el contenido interno */
            display: grid;
            grid-template-columns: 1fr; /* Columna única para móvil */
            grid-template-areas:
                "header"
                "controls"
                "status"
                "video";
            gap: 25px; 
        }
        
        /* CABECERA (H1) */
        h1 {
            grid-area: header;
            font-family: 'Orbitron', sans-serif;
            color: var(--accent-color-1);
            font-size: 2.5em; 
            margin-bottom: 5px;
            text-shadow: 0 0 10px var(--accent-color-1), 0 0 20px var(--accent-color-1);
            letter-spacing: 3px;
        }

        /* ESTADO DEL ROBOT */
        #status {
            grid-area: status;
            padding: 15px;
            font-size: 1.2em;
            font-weight: 700;
            background-color: #0d0d1a; 
            border: 1px solid var(--accent-color-2); 
            border-radius: 8px;
            box-shadow: 0 0 10px var(--accent-color-2);
            transition: all 0.4s ease-in-out;
            color: var(--accent-color-1); 
        }

        /* CONTROLES (Botones) */
        .controls {
            grid-area: controls;
            display: flex;
            flex-direction: column; 
            justify-content: center;
            align-items: center;
            gap: 20px;
        }
        
        button {
            padding: 18px 40px;
            font-size: 1.2em;
            font-family: 'Orbitron', sans-serif;
            font-weight: bold;
            cursor: pointer;
            border: none;
            border-radius: 40px; 
            transition: all 0.3s ease;
            width: 100%;
            max-width: 300px; 
            text-transform: uppercase;
            letter-spacing: 1px;
            position: relative;
            overflow: hidden; 
        }
        
        /* Efectos de Hover/Active (Brillo y Presión) */
        button::before { /* Efecto brillo */
            content: '';
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: rgba(255, 255, 255, 0.2);
            transition: all 0.5s;
            transform: skewX(-20deg);
        }
        button:hover::before { left: 100%; }
        
        #btnStart { background-color: var(--button-green); color: var(--text-dark); box-shadow: 0 0 15px var(--button-green), 0 5px 0px rgba(0, 0, 0, 0.4); }
        #btnStart:hover { background-color: #55ff33; transform: translateY(-2px); box-shadow: 0 0 25px var(--button-green), 0 8px 0px rgba(0, 0, 0, 0.6); }
        #btnStart:active { transform: translateY(1px); box-shadow: 0 0 10px var(--button-green), 0 2px 0px rgba(0, 0, 0, 0.4); }

        #btnRutina2 { background-color: var(--button-blue); color: var(--text-dark); box-shadow: 0 0 15px var(--button-blue), 0 5px 0px rgba(0, 0, 0, 0.4); }
        #btnRutina2:hover { background-color: #33d0ff; transform: translateY(-2px); box-shadow: 0 0 25px var(--button-blue), 0 8px 0px rgba(0, 0, 0, 0.6); }
        #btnRutina2:active { transform: translateY(1px); box-shadow: 0 0 10px var(--button-blue), 0 2px 0px rgba(0, 0, 0, 0.4); }

        #btnReset { background-color: var(--button-red); color: var(--text-light); box-shadow: 0 0 15px var(--button-red), 0 5px 0px rgba(0, 0, 0, 0.4); }
        #btnReset:hover { background-color: #ff3355; transform: translateY(-2px); box-shadow: 0 0 25px var(--button-red), 0 8px 0px rgba(0, 0, 0, 0.6); }
        #btnReset:active { transform: translateY(1px); box-shadow: 0 0 10px var(--button-red), 0 2px 0px rgba(0, 0, 0, 0.4); }


        /* CONTENEDOR DE VIDEO */
        .video-wrapper {
            grid-area: video;
            width: 100%;
            max-width: 650px; 
            margin-left: auto;
            margin-right: auto;
            border: 1px solid var(--accent-color-1); 
            border-radius: 10px;
            box-shadow: 0 0 20px var(--shadow-light); 
            overflow: hidden; 
        }

        .video-wrapper video {
            width: 100%;
            height: auto;
            display: block;
        }

        /* RESPONSIVIDAD PARA DESKTOP */
        @media (min-width: 768px) {
            .container {
                padding: 50px;
                /* Diseño de dos columnas: Controles | Video */
                grid-template-columns: 1fr 1.5fr; 
                grid-template-areas:
                    "header header"
                    "controls video"
                    "status video";
                gap: 30px 40px; 
            }

            h1 {
                font-size: 4em;
            }
            
            .controls {
                flex-direction: column; 
                align-items: stretch; 
            }

            button {
                max-width: none; 
                font-size: 1.3em;
                padding: 20px 45px;
            }
            
            #statusText {
                font-size: 1.4em;
                padding: 20px;
                align-self: end; 
            }
        }
    </style>
</head>
<body>

<div class="container">
    <h1>YARBITO</h1>
    
    <div class="controls">
        <button id="btnStart" onclick="startRoutine()">INICIAR RUTINA 1 (Servos)</button>
        <button id="btnRutina2" onclick="startRoutine2()">INICIAR RUTINA 2 (Carro + Compresor)</button>
        <button id="btnReset" onclick="resetRoutine()">RESET / REPOSO</button>
    </div>

    <p id="statusText">Estado de Yarbito: Inicializando...</p>

    <div class="video-wrapper">
        <!-- Usa un placeholder de video si el archivo 'video solid.mp4' no está disponible -->
        <video controls autoplay muted loop poster="https://placehold.co/650x366/302b63/00f0ff?text=FEED+DE+CÁMARA+DEL+ROBOT">
            <source src="video solid.mp4" type="video/mp4">
            Tu navegador no soporta la etiqueta de video.
        </video>
    </div>
</div>

<script>
    // ****************************************************
    // ** CÓDIGO JAVASCRIPT **
    // ****************************************************
    
    // Configura la IP de tu Arduino/ESP32 con WiFi (MANTÉN ESTA IP ESTÁTICA)
    const ARDUINO_IP = '172.16.1.113'; 

    // Función auxiliar para actualizar el texto de estado en la página
    function updateStatus(text, isSuccess) {
        const statusElement = document.getElementById('statusText');
        statusElement.innerText = 'Estado de Yarbito: ' + text;
        
        // Estilos para el efecto de neón en el estado
        if (isSuccess) {
            statusElement.style.color = 'var(--accent-color-1)';
            statusElement.style.boxShadow = '0 0 10px var(--accent-color-1)';
            statusElement.style.borderColor = 'var(--accent-color-1)';
        } else {
            statusElement.style.color = 'var(--button-red)';
            statusElement.style.boxShadow = '0 0 10px var(--button-red)';
            statusElement.style.borderColor = 'var(--button-red)';
        }
    }

    /**
     * Función genérica para enviar comandos HTTP GET al Arduino.
     * @param {string} command El path del comando (ej: '/start', '/rutina2')
     * @param {string} successMessage Mensaje a mostrar si el comando es exitoso.
     * @param {string} failureMessage Mensaje a mostrar si el comando falla.
     */
    function sendCommand(command, successMessage, failureMessage) {
        updateStatus('Enviando comando ' + command.replace('/', '') + '...', true);
        
        // *****************************************************************
        // 🛑 CORRECCIÓN CLAVE: La URL ahora incluye la IP Y el comando.
        const url = 'http://' + ARDUINO_IP + command; 
        // *****************************************************************

        fetch(url) 
            .then(response => {
                if (response.ok) {
                    updateStatus(successMessage, true);
                } else {
                    updateStatus(failureMessage + ' (Cód. HTTP: ' + response.status + ')', false);
                }
            })
            .catch(error => {
                // Muestra la IP completa y el error de red
                updateStatus('ERROR: No se pudo conectar a ' + ARDUINO_IP + '. Verifique la IP y la red.', false);
                console.error('Error de conexión:', error); 
            });
    }

    // ****************************************************
    // ** COMANDOS DE RUTINA **
    // ****************************************************
    
    function startRoutine() {
        sendCommand('/start', 'Rutina 1 (Movimiento de Servos) activada.', 'Fallo al iniciar Rutina 1.');
    }

    function startRoutine2() {
        sendCommand('/rutina2', 'Rutina 2 (Carro y Compresor) activada.', 'Fallo al iniciar Rutina 2.');
    }

    function resetRoutine() {
        sendCommand('/reset', 'Yarbito restablecido y en reposo.', 'Fallo al ejecutar RESET.');
    }

    // Inicializa el estado al cargar la página
    window.onload = function() {
        updateStatus('Listo para operación. IP configurada: ' + ARDUINO_IP, true);
    };
</script>

</body>
</html>