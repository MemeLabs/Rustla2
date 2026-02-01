// @flow

import React from "react";
import cs from "classnames";

import "../css/ImageModal";
import { connect } from "react-redux";
import { imageModalSrc } from "../actions";

const ImageModal = ({ dispatch, src, onClose }) => (
  <div
    className="image-modal"
    tabIndex={-1}
    onClick={onClose}
    onKeyDown={(e) => {
      if (e.key === "Escape") dispatch(imageModalSrc(null));
    }}
  >
    <div
      className="image-modal__backdrop"
      onClick={() => {
        dispatch(imageModalSrc(null));
      }}
    ></div>

    <div className="image-modal__content" onClick={(e) => e.stopPropagation()}>
      <div className="image-modal__toolbar">
        <button
          className="image-modal__btn button-orange"
          tabIndex={0}
           onClick={() => {
           window.open(src,'_blank')
           }}
        >
          <span className="glyphicon glyphicon-new-window" />
        </button>
        <button
          className="image-modal__btn button-orange"
          tabIndex={0}
          onClick={() => navigator.clipboard.writeText(src)}
        >
          <span className="glyphicon glyphicon-duplicate" />
        </button>

        <button
          className="image-modal__btn image-modal__btn--close button-orange"
          aria-label="Close"
          tabIndex={0}
          autoFocus
          onClick={() => {
            dispatch(imageModalSrc(null));
          }}
        >
          <span className="glyphicon glyphicon-remove" />
        </button>
      </div>

      <img src={src} alt="" />
    </div>
  </div>
);

export default connect()(ImageModal);
