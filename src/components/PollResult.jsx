// @flow

import React from 'react';
import { connect } from 'react-redux';
import idx from 'idx';

import { beginPollingPoll } from '../actions';
import MainLayout from './MainLayout';

import type { Poll } from '../records/polls';

const PollResultRow = ({
  value,
  votes,
  percent,
}: {
  value: string;
  votes: number;
  percent: number;
}) => (
  <React.Fragment>
    <div className='poll-result-label-row'>
      <span className='poll-result-label'>{value}</span>
      <span className='poll-result-count'>
        {votes} {votes === 1 ? 'vote' : 'votes'}
      </span>
    </div>
    <div className='poll-result-chart-row'>
      <div className='poll-result-bar-container'>
        <div
          className='poll-result-bar'
          style={{'width': `${percent}%`}}
        />
      </div>
      <span className='poll-result-percent'>
        {Math.round(percent)}%
      </span>
    </div>
  </React.Fragment>
);

const PollResultChart = ({ poll: { subject, options } }: { poll: Poll }) => {
  const maxValue = Object.keys(options)
    .reduce((maxValue, key) => Math.max(maxValue, options[key]), 0);

  const optionRows = Object.keys(options)
    .map((key: string): [string, number] => [key, options[key]])
    .sort((a, b) => b[1] - a[1])
    .map(([value, votes], i) => (
      <PollResultRow
        key={i}
        value={value}
        votes={votes}
        percent={!maxValue ? 0 : (votes / maxValue) * 100}
      />
    ));

  return (
    <div>
      <h1 className='poll-title'>{subject}</h1>
      {optionRows}
    </div>
  );
};


type PollResultProps = {
  id: string;
  poll?: Poll;
  history: any;
  beginPollingPoll: () => void;
  beginPollingPoll: (string) => () => void;
};

class PollResult extends React.Component<PollResultProps> {
  stopPolling: () => void;

  componentDidMount() {
    this.stopPolling = this.props.beginPollingPoll(this.props.id);
  }

  componentWillUnmount() {
    this.stopPolling();
  }

  render() {
    const { poll, history } = this.props;

    const pollChart = poll && !poll.loading
      ? <PollResultChart poll={poll} />
      : <h4 className='poll-loading'>Loading...</h4>;

    return (
      <MainLayout history={history}>
        <div className='container'>
          {pollChart}
        </div>
      </MainLayout>
    );
  }
}

function mapDispatchToProps(dispatch) {
  return {
    beginPollingPoll: function(id) {
      return dispatch(beginPollingPoll(id));
    },
  };
}

export default connect(
  (state, ownProps) => {
    const id = ownProps.match.params.poll;
    return {
      id,
      poll: idx(state, _ => _.polls[id]),
    };
  },
  mapDispatchToProps,
)(PollResult);
