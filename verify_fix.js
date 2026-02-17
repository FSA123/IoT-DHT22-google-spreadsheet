function validateInput(temperature, humidity) {
    if (temperature === undefined || humidity === undefined ||
        temperature === "" || humidity === "") {
        return { valid: false, error: "Missing or empty parameters" };
    }

    const tempNum = Number(temperature);
    const humNum = Number(humidity);

    if (isNaN(tempNum) || !isFinite(tempNum) || isNaN(humNum) || !isFinite(humNum)) {
        return { valid: false, error: "Invalid numeric data" };
    }

    return { valid: true, temperature: tempNum, humidity: humNum };
}

const testCases = [
    { name: "Valid integers", temp: "25", hum: "50", expectedValid: true },
    { name: "Valid floats", temp: "25.5", hum: "50.1", expectedValid: true },
    { name: "Negative temperature", temp: "-5", hum: "40", expectedValid: true },
    { name: "Formula injection =", temp: "=SUM(1,2)", hum: "50", expectedValid: false },
    { name: "Formula injection + (invalid number)", temp: "+1+1", hum: "50", expectedValid: false },
    { name: "Formula injection - (invalid number)", temp: "-1+1", hum: "50", expectedValid: false },
    { name: "Positive sign (valid number)", temp: "+25", hum: "50", expectedValid: true },
    { name: "Potential injection @", temp: "@SUM", hum: "50", expectedValid: false },
    { name: "Empty string", temp: "", hum: "50", expectedValid: false },
    { name: "Missing parameter", temp: undefined, hum: "50", expectedValid: false },
    { name: "Not a number", temp: "abc", hum: "50", expectedValid: false },
    { name: "Infinity", temp: "Infinity", hum: "50", expectedValid: false },
];

let failed = false;
testCases.forEach(tc => {
    const result = validateInput(tc.temp, tc.hum);
    if (result.valid === tc.expectedValid) {
        console.log(`PASS: ${tc.name}`);
    } else {
        console.log(`FAIL: ${tc.name} - Expected ${tc.expectedValid}, got ${result.valid}. Result:`, result);
        failed = true;
    }
});

if (failed) {
    process.exit(1);
} else {
    console.log("All tests passed!");
}
