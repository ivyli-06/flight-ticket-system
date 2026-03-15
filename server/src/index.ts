import cors from "cors";
import "dotenv/config";
import express from "express";

import { authRouter } from "./routes/auth.js";
import { flightsRouter } from "./routes/flights.js";
import { ordersRouter } from "./routes/orders.js";

const app = express();

app.use(cors({
  origin: process.env.CORS_ORIGIN ?? "*",
  credentials: false
}));

app.use(express.json());

app.get("/health", (_req, res) => res.json({ ok: true }));

app.use("/api/auth", authRouter);
app.use("/api/flights", flightsRouter);
app.use("/api/orders", ordersRouter);

const port = Number(process.env.PORT ?? 3000);
app.listen(port, () => {
  console.log(`Server listening on http://127.0.0.1:${port}`);
});