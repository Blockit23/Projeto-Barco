import { initializeApp } from "https://www.gstatic.com/firebasejs/9.22.1/firebase-app.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/9.22.1/firebase-database.js";

// Configuração Firebase
const firebaseConfig = {
    apiKey: "AIzaSyBXW84-0Hbm0Xc9CrMRJt8qWkjwI6WOOwg",
    authDomain: "barco-teste.firebaseapp.com",
    databaseURL: "https://barco-teste-default-rtdb.firebaseio.com",
    projectId: "barco-teste",
    storageBucket: "barco-teste.appspot.com",
    messagingSenderId: "1079199833752",
    appId: "1:1079199833752:web:3d666b31c611b5fe6f3aed"
};

// da play no fire
const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

// puxa as info do codigo de piton
const tempRef = ref(db, "arduino/temperatura");


const elem = document.getElementById("temperatura");
if (elem) {
    elem.textContent = "Carregando...";
}

// Atualiza o tempo todo
onValue(tempRef, (snapshot) => {
    if (!elem) return;

    const valor = snapshot.val();

    if (valor !== null) {
        elem.textContent = parseFloat(valor).toFixed(2) + " °C";
    } else {
        elem.textContent = "Carregando...";
    }
});

