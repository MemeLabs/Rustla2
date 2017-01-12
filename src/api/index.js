import express from 'express';

const router = express.Router();

router.use((req, res) => {
  res.json({
    memes: true
  });
});

export default router;
