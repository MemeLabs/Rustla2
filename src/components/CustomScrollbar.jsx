// @flow

import React from 'react';
import Scrollbars from 'react-custom-scrollbars';

type RenderProps = { style: $Shape<CSSStyleDeclaration> };

const viewStyle = {
  padding: 1,
};

function renderView({ style }: RenderProps) {
  return <div className="box" style={{ ...style, ...viewStyle }} />;
}

const thumbStyle: $Shape<CSSStyleDeclaration> = {
  backgroundColor: 'rgb(55, 55, 55)'
};

function renderThumb({ style }: RenderProps) {
  return <div style={{ ...style, ...thumbStyle }} />;
}

const ColoredScrollbars = (props: mixed) =>
  <Scrollbars
    renderView={renderView}
    renderThumbVertical={renderThumb}
    {...props}
  />;

export default ColoredScrollbars;