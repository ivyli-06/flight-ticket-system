import type { NextFunction, Request, Response } from "express";
import { verifyToken, type JwtUser } from "../auth.js";

declare global {
  namespace Express {
    interface Request {
      user?: JwtUser;
    }
  }
}

export function authRequired(req: Request, res: Response, next: NextFunction) {
  const auth = req.header("Authorization") ?? "";
  const [scheme, token] = auth.split(" ");

  if (scheme !== "Bearer" || !token) {
    return res.status(401).json({ error: "Missing Authorization: Bearer token" });
  }

  try {
    req.user = verifyToken(token);
    next();
  } catch {
    return res.status(401).json({ error: "Invalid token" });
  }
}