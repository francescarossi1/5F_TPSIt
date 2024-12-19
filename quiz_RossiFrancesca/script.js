document.addEventListener("DOMContentLoaded", () => {
    const timerElement = document.getElementById("timer");
    const urlParams = new URLSearchParams(window.location.search);
    const domandaId = urlParams.get("domanda");
    const testoId = urlParams.get("testo");

    // Timer
    let startTime = new Date(localStorage.getItem("startTime") || new Date());
    localStorage.setItem("startTime", startTime);

    function updateTimer() {
        const elapsedTime = new Date(new Date() - startTime);
        const hours = String(elapsedTime.getUTCHours()).padStart(2, "0");
        const minutes = String(elapsedTime.getUTCMinutes()).padStart(2, "0");
        const seconds = String(elapsedTime.getUTCSeconds()).padStart(2, "0");
        if (timerElement) {
            timerElement.textContent = `Tempo: ${hours}:${minutes}:${seconds}`;
        }
    }
    setInterval(updateTimer, 1000);

    // Funzione per contare le parole
    function countWords(text) {
        const words = text.trim().split(/\s+/);
        return words.length === 1 && words[0] === "" ? 0 : words.length;
    }

    // Carica dati JSON
    fetch("domande.json")
        .then((response) => response.json())
        .then((data) => {
            // Domande aperte
            if (domandaId && data.domandeAperte[domandaId]) {
                const questionTitle = document.getElementById("question-title");
                const questionText = document.getElementById("question-text");
                const answerInput = document.getElementById("answer");
                const wordCountElement = document.getElementById("word-count");

                if (questionTitle) questionTitle.textContent = `Domanda ${domandaId}`;
                if (questionText) questionText.textContent = data.domandeAperte[domandaId];

                // Ripristina la risposta salvata
                if (answerInput) {
                    answerInput.value = loadAnswer(`domandaAperta_${domandaId}`);
                    answerInput.addEventListener("input", (event) => {
                        saveAnswer(`domandaAperta_${domandaId}`, event.target.value);
                        // Conta e aggiorna il numero di parole
                        const wordCount = countWords(event.target.value);
                        if (wordCountElement) {
                            wordCountElement.textContent = `Parole scritte: ${wordCount}`;
                        }
                    });
                }
            }

            // Testi con domande a crocette
            if (testoId && data.testi[testoId]) {
                const testo = data.testi[testoId];
                const testTitle = document.getElementById("test-title");
                const testContent = document.getElementById("test-content");

                if (testTitle) testTitle.textContent = testo.titolo;
                if (testContent) {
                    testContent.innerHTML = `<p>${testo.testo}</p>`;
                    testo.domande.forEach((d, index) => {
                        const domandaKey = `testo_${testoId}_domanda_${index}`;
                        const savedAnswer = loadAnswer(domandaKey);

                        const domandaElement = document.createElement("div");
                        domandaElement.className = "question";
                        domandaElement.innerHTML = `
                            <h2>Domanda ${index + 1}: ${d.domanda}</h2>
                            ${d.opzioni
                                .map(
                                    (opzione) => `
                                <div>
                                    <label>
                                        <input type="radio" name="${domandaKey}" value="${opzione}" ${
                                            savedAnswer === opzione ? "checked" : ""
                                        }>
                                        ${opzione}
                                    </label>
                                </div>
                            `
                                )
                                .join("")}
                        `;

                        // Aggiungi event listener per salvare la risposta
                        domandaElement.querySelectorAll("input[type='radio']").forEach((input) => {
                            input.addEventListener("change", (event) => {
                                saveAnswer(domandaKey, event.target.value);
                            });
                        });

                        testContent.appendChild(domandaElement);
                    });
                }
            }
        })
        .catch((error) => console.error("Errore nel caricamento del file JSON:", error));

    // Funzione per salvare la risposta nel localStorage
    function saveAnswer(key, value) {
        const savedAnswers = JSON.parse(localStorage.getItem("answers")) || {};
        savedAnswers[key] = value;
        localStorage.setItem("answers", JSON.stringify(savedAnswers));
    }

    // Funzione per caricare la risposta dal localStorage
    function loadAnswer(key) {
        const savedAnswers = JSON.parse(localStorage.getItem("answers")) || {};
        return savedAnswers[key] || "";
    }

    // Funzione per creare il file di testo con le risposte
    function createDownloadFile() {
        const savedAnswers = JSON.parse(localStorage.getItem("answers")) || {};
        let fileContent = "Risposte:\n\n";

        // Aggiungi domande aperte
        for (let key in savedAnswers) {
            fileContent += `${key}: ${savedAnswers[key]}\n`;
        }

        // Aggiungi tempo del timer
        const elapsedTime = new Date(new Date() - startTime);
        const hours = String(elapsedTime.getUTCHours()).padStart(2, "0");
        const minutes = String(elapsedTime.getUTCMinutes()).padStart(2, "0");
        const seconds = String(elapsedTime.getUTCSeconds()).padStart(2, "0");
        fileContent += `\nTempo trascorso: ${hours}:${minutes}:${seconds}`;

        // Crea un Blob con il contenuto e un link per il download
        const blob = new Blob([fileContent], { type: "text/plain;charset=utf-8" });
        const link = document.createElement("a");
        link.href = URL.createObjectURL(blob);
        link.download = "risposte.txt"; // Nome del file
        link.click();

        // Azzera il timer e salva il nuovo orario di inizio
        startTime = new Date();
        localStorage.setItem("startTime", startTime);

        // Aggiorna immediatamente il timer
        updateTimer();

        // Cancella i dati inseriti
        localStorage.removeItem("answers"); // Rimuove tutte le risposte salvate

        // Pulisce i campi di input
        document.querySelectorAll("textarea, input[type='radio']").forEach((input) => {
            if (input.type === "radio") {
                input.checked = false;
            } else {
                input.value = "";
            }
        });
    }

    // Aggiungi event listener al pulsante per inviare le risposte
    const submitButton = document.getElementById("submit-answers");
    if (submitButton) {
        submitButton.addEventListener("click", createDownloadFile);
    }
});
